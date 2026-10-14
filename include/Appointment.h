#pragma once
// A booking between one patient and one doctor in a one-hour slot.

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "Date.h"

enum class AppointmentStatus { Scheduled, Completed, Cancelled };

class Appointment {
private:
    int id_;
    int patientId_;
    int doctorId_;
    Date date_;
    int slot_;  // start hour, 9..16 (last slot ends at 17:00)
    AppointmentStatus status_;

public:
    static constexpr int FIRST_SLOT = 9;
    static constexpr int LAST_SLOT = 16;

    Appointment(int id, int patientId, int doctorId, const Date& date, int slot,
                AppointmentStatus status = AppointmentStatus::Scheduled);

    int getId() const { return id_; }
    int getPatientId() const { return patientId_; }
    int getDoctorId() const { return doctorId_; }
    const Date& getDate() const { return date_; }
    int getSlot() const { return slot_; }
    AppointmentStatus getStatus() const { return status_; }
    bool isScheduled() const { return status_ == AppointmentStatus::Scheduled; }

    void complete();
    void cancel();

    // Same date and same slot, and both still active.
    bool clashesWith(const Date& date, int slot) const;

    std::string slotLabel() const;
    static std::string statusName(AppointmentStatus status);

    std::string serialize() const;
    static std::shared_ptr<Appointment> fromRecord(const std::vector<std::string>& fields);

    bool operator<(const Appointment& other) const;  // chronological order
    friend std::ostream& operator<<(std::ostream& os, const Appointment& appt);
};
