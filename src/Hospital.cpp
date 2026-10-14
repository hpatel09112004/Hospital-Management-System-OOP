#include "Hospital.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <utility>

#include "Exceptions.h"
#include "PatientFactory.h"
#include "Utils.h"

namespace fs = std::filesystem;

Hospital::Hospital(std::string name) : name_(std::move(name)) {}

// ------------------------------------------------------------ registration

std::shared_ptr<Doctor> Hospital::addDoctor(const std::string& name, int age,
                                            const std::string& phone,
                                            const std::string& specialization, double fee) {
    auto doctor = std::make_shared<Doctor>(nextDoctorId_, name, age, phone, specialization, fee);
    doctors_.add(doctor);
    ++nextDoctorId_;  // only advance once the object was built and stored
    return doctor;
}

std::shared_ptr<OutPatient> Hospital::registerOutPatient(const std::string& name, int age,
                                                         const std::string& phone,
                                                         const std::string& ailment) {
    auto patient = std::make_shared<OutPatient>(nextPatientId_, name, age, phone, ailment);
    patients_.add(patient);  // shared_ptr<OutPatient> -> shared_ptr<Patient> (upcast)
    ++nextPatientId_;
    return patient;
}

std::shared_ptr<InPatient> Hospital::admitInPatient(const std::string& name, int age,
                                                    const std::string& phone,
                                                    const std::string& ailment, RoomType room,
                                                    int days) {
    auto patient =
        std::make_shared<InPatient>(nextPatientId_, name, age, phone, ailment, room, days);
    patients_.add(patient);
    ++nextPatientId_;
    return patient;
}

// ------------------------------------------------------------ lookups

std::vector<std::shared_ptr<Doctor>> Hospital::searchDoctors(const std::string& query) const {
    const std::string needle = util::toLower(util::trim(query));
    return doctors_.findIf([&needle](const Doctor& d) {
        return util::toLower(d.getSpecialization()).find(needle) != std::string::npos;
    });
}

std::vector<std::shared_ptr<Appointment>> Hospital::appointmentsSorted(int doctorId) const {
    auto list = appointments_.findIf(
        [doctorId](const Appointment& a) { return doctorId == 0 || a.getDoctorId() == doctorId; });
    std::sort(list.begin(), list.end(),
              [](const auto& a, const auto& b) { return *a < *b; });  // uses operator<
    return list;
}

std::string Hospital::patientLabel(int patientId) const {
    return patients_.exists(patientId) ? patients_.get(patientId)->getName()
                                       : std::string("[discharged]");
}

std::string Hospital::doctorLabel(int doctorId) const {
    return doctors_.exists(doctorId) ? "Dr. " + doctors_.get(doctorId)->getName()
                                     : std::string("[unknown doctor]");
}

// ------------------------------------------------------------ workflow

void Hospital::assignDoctor(int patientId, int doctorId) {
    auto patient = patients_.get(patientId);
    doctors_.get(doctorId);  // throws NotFoundException if missing
    patient->assignDoctor(doctorId);
}

std::shared_ptr<Appointment> Hospital::bookAppointment(int patientId, int doctorId,
                                                       const Date& date, int slot) {
    auto patient = patients_.get(patientId);
    auto doctor = doctors_.get(doctorId);

    // Build first so the slot is validated before checking clashes.
    auto appt = std::make_shared<Appointment>(nextAppointmentId_, patientId, doctorId, date, slot);

    for (const auto& existing : appointments_.all()) {
        if (!existing->clashesWith(date, slot)) continue;
        if (existing->getDoctorId() == doctorId) {
            throw SchedulingConflictException(doctorLabel(doctorId) + " is already booked on " +
                                              date.toString() + " at " + appt->slotLabel());
        }
        if (existing->getPatientId() == patientId) {
            throw SchedulingConflictException(patient->getName() +
                                              " already has an appointment on " +
                                              date.toString() + " at " + appt->slotLabel());
        }
    }

    appointments_.add(appt);
    ++nextAppointmentId_;
    if (!patient->hasDoctor()) patient->assignDoctor(doctorId);
    return appt;
}

void Hospital::completeAppointment(int appointmentId, const std::string& notes) {
    auto appt = appointments_.get(appointmentId);
    if (!patients_.exists(appt->getPatientId())) {
        throw ValidationException("the patient for this appointment has been discharged");
    }
    auto patient = patients_.get(appt->getPatientId());
    appt->complete();

    std::string entry = appt->getDate().toString() + " " + doctorLabel(appt->getDoctorId());
    const std::string clean = util::sanitize(notes);
    if (!clean.empty()) entry += ": " + clean;
    patient->addHistory(entry);
    patient->recordVisit();  // virtual: OutPatient counts it, InPatient ignores it
}

void Hospital::cancelAppointment(int appointmentId) {
    appointments_.get(appointmentId)->cancel();
}

void Hospital::orderLabTests(int patientId, int count) {
    auto patient = patients_.get(patientId);
    // Downcast with RTTI: only out-patients are billed for lab tests here.
    auto outPatient = std::dynamic_pointer_cast<OutPatient>(patient);
    if (!outPatient) {
        throw ValidationException(patient->getName() + " is an in-patient; lab work is part of " +
                                  "the ward stay");
    }
    outPatient->addLabTests(count);
}

void Hospital::extendStay(int patientId, int extraDays) {
    auto patient = patients_.get(patientId);
    auto inPatient = std::dynamic_pointer_cast<InPatient>(patient);
    if (!inPatient) {
        throw ValidationException(patient->getName() + " is an out-patient and is not admitted");
    }
    inPatient->extendStay(extraDays);
}

// ------------------------------------------------------------ billing

Bill Hospital::generateBill(int patientId) const {
    auto patient = patients_.get(patientId);
    if (!patient->hasDoctor()) {
        throw ValidationException("assign a doctor to " + patient->getName() + " before billing");
    }
    auto doctor = doctors_.get(patient->getDoctorId());

    // Runtime polymorphism: the same call runs InPatient's or OutPatient's version.
    Bill bill = patient->generateBill(doctor->getFee());
    if (patient->getAge() >= SENIOR_AGE) {
        bill.applyDiscount(SENIOR_DISCOUNT_PERCENT, "Senior citizen discount");
    }
    return bill;
}

Bill Hospital::dischargePatient(int patientId) {
    Bill finalBill = generateBill(patientId);  // throws before anything changes
    for (const auto& appt : appointments_.all()) {
        if (appt->getPatientId() == patientId && appt->isScheduled()) appt->cancel();
    }
    patients_.remove(patientId);
    return finalBill;
}

// ------------------------------------------------------------ reporting

HospitalStats Hospital::stats() const {
    HospitalStats s;
    s.doctors = doctors_.size();
    for (const auto& p : patients_.all()) {
        if (p->getType() == "IN") ++s.inPatients; else ++s.outPatients;
        if (p->hasDoctor() && doctors_.exists(p->getDoctorId())) {
            s.outstandingBilling += generateBill(p->getId()).total();
        }
    }
    for (const auto& a : appointments_.all()) {
        switch (a->getStatus()) {
            case AppointmentStatus::Scheduled: ++s.scheduled; break;
            case AppointmentStatus::Completed: ++s.completed; break;
            case AppointmentStatus::Cancelled: ++s.cancelled; break;
        }
    }
    return s;
}

void Hospital::printDoctors(std::ostream& os) const {
    if (doctors_.empty()) { os << "  (no doctors yet)\n"; return; }
    for (const auto& d : doctors_.all()) os << "  " << *d << '\n';
}

void Hospital::printPatients(std::ostream& os) const {
    if (patients_.empty()) { os << "  (no patients yet)\n"; return; }
    for (const auto& p : patients_.all()) os << "  " << *p << '\n';  // virtual display()
}

void Hospital::printAppointments(std::ostream& os, int doctorId) const {
    const auto list = appointmentsSorted(doctorId);
    if (list.empty()) { os << "  (no appointments)\n"; return; }
    for (const auto& a : list) {
        os << "  " << *a << "  [" << patientLabel(a->getPatientId()) << " with "
           << doctorLabel(a->getDoctorId()) << "]\n";
    }
}

void Hospital::printHistory(std::ostream& os, int patientId) const {
    auto patient = patients_.get(patientId);
    os << "  Medical history of " << patient->getName() << ":\n";
    if (patient->getHistory().empty()) { os << "    (no visits recorded)\n"; return; }
    int n = 1;
    for (const auto& entry : patient->getHistory()) os << "    " << n++ << ". " << entry << '\n';
}

void Hospital::printStats(std::ostream& os) const {
    const HospitalStats s = stats();
    os << "  Hospital            : " << name_ << '\n'
       << "  Doctors             : " << s.doctors << '\n'
       << "  Patients            : " << s.inPatients + s.outPatients << " (" << s.inPatients
       << " in-patient, " << s.outPatients << " out-patient)\n"
       << "  Appointments        : " << s.scheduled << " scheduled, " << s.completed
       << " completed, " << s.cancelled << " cancelled\n"
       << "  Outstanding billing : " << util::money(s.outstandingBilling) << '\n'
       << "  Live Person objects : " << Person::population() << '\n';
}

// ------------------------------------------------------------ persistence

namespace {
void writeLines(const fs::path& file, const std::vector<std::string>& lines) {
    std::ofstream out(file);
    if (!out) throw HospitalException("cannot write " + file.string());
    for (const auto& line : lines) out << line << '\n';
}
}  // namespace

void Hospital::saveToDirectory(const std::string& directory) const {
    fs::create_directories(directory);
    const fs::path dir(directory);

    std::vector<std::string> lines;
    for (const auto& d : doctors_.all()) lines.push_back(d->serialize());
    writeLines(dir / "doctors.txt", lines);

    lines.clear();
    for (const auto& p : patients_.all()) lines.push_back(p->serialize());  // virtual
    writeLines(dir / "patients.txt", lines);

    lines.clear();
    for (const auto& a : appointments_.all()) lines.push_back(a->serialize());
    writeLines(dir / "appointments.txt", lines);
}

int Hospital::loadFromDirectory(const std::string& directory, std::ostream& warnings) {
    const fs::path dir(directory);
    int loaded = 0;

    // Reads one file, handing each non-empty line's fields to `store`.
    auto readFile = [&](const std::string& fileName, auto store) {
        std::ifstream in(dir / fileName);
        if (!in) return;  // first run: nothing saved yet
        std::string line;
        int lineNo = 0;
        while (std::getline(in, line)) {
            ++lineNo;
            if (util::trim(line).empty()) continue;
            try {
                store(util::split(line, '|'));
                ++loaded;
            } catch (const std::exception& e) {
                warnings << "  Skipped " << fileName << " line " << lineNo << ": " << e.what()
                         << '\n';
            }
        }
    };

    readFile("doctors.txt", [&](const auto& f) { doctors_.add(Doctor::fromRecord(f)); });
    readFile("patients.txt", [&](const auto& f) { patients_.add(PatientFactory::fromRecord(f)); });
    readFile("appointments.txt",
             [&](const auto& f) { appointments_.add(Appointment::fromRecord(f)); });

    // Continue numbering after the highest saved ID.
    nextDoctorId_ = std::max(nextDoctorId_, doctors_.maxId() + 1);
    nextPatientId_ = std::max(nextPatientId_, patients_.maxId() + 1);
    nextAppointmentId_ = std::max(nextAppointmentId_, appointments_.maxId() + 1);
    return loaded;
}
