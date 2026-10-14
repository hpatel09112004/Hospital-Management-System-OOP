#pragma once
// Facade over the whole system. Hospital HAS-A set of repositories
// (composition) and enforces the business rules that span several classes:
// scheduling conflicts, billing, discharge and file persistence.

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "Appointment.h"
#include "Bill.h"
#include "Doctor.h"
#include "InPatient.h"
#include "OutPatient.h"
#include "Patient.h"
#include "Repository.h"

struct HospitalStats {
    std::size_t doctors = 0;
    std::size_t inPatients = 0;
    std::size_t outPatients = 0;
    std::size_t scheduled = 0;
    std::size_t completed = 0;
    std::size_t cancelled = 0;
    double outstandingBilling = 0;
};

class Hospital {
private:
    std::string name_;
    Repository<Doctor> doctors_{"Doctor"};
    Repository<Patient> patients_{"Patient"};
    Repository<Appointment> appointments_{"Appointment"};

    int nextDoctorId_ = 101;
    int nextPatientId_ = 1001;
    int nextAppointmentId_ = 1;

    std::string patientLabel(int patientId) const;
    std::string doctorLabel(int doctorId) const;

public:
    static constexpr int SENIOR_AGE = 60;
    static constexpr double SENIOR_DISCOUNT_PERCENT = 10.0;

    explicit Hospital(std::string name);

    const std::string& getName() const { return name_; }

    // --- Registration ---
    std::shared_ptr<Doctor> addDoctor(const std::string& name, int age, const std::string& phone,
                                      const std::string& specialization, double fee);
    std::shared_ptr<OutPatient> registerOutPatient(const std::string& name, int age,
                                                   const std::string& phone,
                                                   const std::string& ailment);
    std::shared_ptr<InPatient> admitInPatient(const std::string& name, int age,
                                              const std::string& phone,
                                              const std::string& ailment, RoomType room,
                                              int days);

    // --- Lookups ---
    std::shared_ptr<Doctor> getDoctor(int id) const { return doctors_.get(id); }
    std::shared_ptr<Patient> getPatient(int id) const { return patients_.get(id); }
    std::shared_ptr<Appointment> getAppointment(int id) const { return appointments_.get(id); }
    std::vector<std::shared_ptr<Doctor>> searchDoctors(const std::string& specialization) const;
    std::vector<std::shared_ptr<Appointment>> appointmentsSorted(int doctorId = 0) const;

    // --- Clinical workflow ---
    void assignDoctor(int patientId, int doctorId);
    std::shared_ptr<Appointment> bookAppointment(int patientId, int doctorId, const Date& date,
                                                 int slot);
    void completeAppointment(int appointmentId, const std::string& notes);
    void cancelAppointment(int appointmentId);
    void orderLabTests(int patientId, int count);
    void extendStay(int patientId, int extraDays);

    // --- Billing ---
    Bill generateBill(int patientId) const;
    Bill dischargePatient(int patientId);

    // --- Reporting ---
    HospitalStats stats() const;
    void printDoctors(std::ostream& os) const;
    void printPatients(std::ostream& os) const;
    void printAppointments(std::ostream& os, int doctorId = 0) const;
    void printHistory(std::ostream& os, int patientId) const;
    void printStats(std::ostream& os) const;

    // --- Persistence (plain text files, one record per line) ---
    void saveToDirectory(const std::string& directory) const;
    int loadFromDirectory(const std::string& directory, std::ostream& warnings);
};
