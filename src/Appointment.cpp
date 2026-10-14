#include "Appointment.h"

#include <iomanip>
#include <ostream>
#include <sstream>
#include <tuple>

#include "Exceptions.h"

Appointment::Appointment(int id, int patientId, int doctorId, const Date& date, int slot,
                         AppointmentStatus status)
    : id_(id), patientId_(patientId), doctorId_(doctorId), date_(date), slot_(slot),
      status_(status) {
    if (slot < FIRST_SLOT || slot > LAST_SLOT) {
        throw ValidationException("slot must be an hour between " +
                                  std::to_string(FIRST_SLOT) + " and " +
                                  std::to_string(LAST_SLOT));
    }
}

void Appointment::complete() {
    if (!isScheduled()) {
        throw ValidationException("appointment #" + std::to_string(id_) + " is already " +
                                  statusName(status_));
    }
    status_ = AppointmentStatus::Completed;
}

void Appointment::cancel() {
    if (!isScheduled()) {
        throw ValidationException("appointment #" + std::to_string(id_) + " is already " +
                                  statusName(status_));
    }
    status_ = AppointmentStatus::Cancelled;
}

bool Appointment::clashesWith(const Date& date, int slot) const {
    return isScheduled() && date_ == date && slot_ == slot;
}

std::string Appointment::slotLabel() const {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(2) << slot_ << ":00-" << std::setw(2) << slot_ + 1
       << ":00";
    return os.str();
}

std::string Appointment::statusName(AppointmentStatus status) {
    switch (status) {
        case AppointmentStatus::Scheduled: return "Scheduled";
        case AppointmentStatus::Completed: return "Completed";
        case AppointmentStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

std::string Appointment::serialize() const {
    std::ostringstream os;
    os << id_ << '|' << patientId_ << '|' << doctorId_ << '|' << date_.toString() << '|'
       << slot_ << '|' << static_cast<int>(status_);
    return os.str();
}

std::shared_ptr<Appointment> Appointment::fromRecord(const std::vector<std::string>& f) {
    if (f.size() != 6) throw ValidationException("appointment record needs 6 fields");
    const int status = std::stoi(f[5]);
    if (status < 0 || status > 2) throw ValidationException("bad appointment status");
    return std::make_shared<Appointment>(std::stoi(f[0]), std::stoi(f[1]), std::stoi(f[2]),
                                         Date::parse(f[3]), std::stoi(f[4]),
                                         static_cast<AppointmentStatus>(status));
}

bool Appointment::operator<(const Appointment& other) const {
    if (date_ != other.date_) return date_ < other.date_;
    return std::tie(slot_, id_) < std::tie(other.slot_, other.id_);
}

std::ostream& operator<<(std::ostream& os, const Appointment& a) {
    os << "#" << a.id_ << " | " << a.date_ << ' ' << a.slotLabel() << " | Patient "
       << a.patientId_ << " | Doctor " << a.doctorId_ << " | "
       << Appointment::statusName(a.status_);
    return os;
}
