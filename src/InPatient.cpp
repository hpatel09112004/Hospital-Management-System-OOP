#include "InPatient.h"

#include <ostream>

#include "Exceptions.h"

InPatient::InPatient(int id, const std::string& name, int age, const std::string& phone,
                     const std::string& ailment, RoomType room, int daysAdmitted)
    : Patient(id, name, age, phone, ailment), room_(room), daysAdmitted_(0) {
    if (daysAdmitted < 1) throw ValidationException("an in-patient stays at least 1 day");
    daysAdmitted_ = daysAdmitted;
}

void InPatient::extendStay(int extraDays) {
    if (extraDays < 1) throw ValidationException("extra days must be at least 1");
    daysAdmitted_ += extraDays;
}

double InPatient::roomRate(RoomType room) {
    switch (room) {
        case RoomType::General: return 1500.0;
        case RoomType::SemiPrivate: return 3000.0;
        case RoomType::Private: return 5000.0;
    }
    return 0.0;
}

std::string InPatient::roomName(RoomType room) {
    switch (room) {
        case RoomType::General: return "General";
        case RoomType::SemiPrivate: return "Semi-Private";
        case RoomType::Private: return "Private";
    }
    return "Unknown";
}

RoomType InPatient::roomFromInt(int value) {
    if (value < 1 || value > 3) throw ValidationException("room type must be 1, 2 or 3");
    return static_cast<RoomType>(value);
}

std::string InPatient::serialize() const {
    return serializeCommon() + '|' + std::to_string(static_cast<int>(room_)) + '|' +
           std::to_string(daysAdmitted_);
}

Bill InPatient::generateBill(double consultationFee) const {
    Bill bill("In-Patient " + name_ + " (ID " + std::to_string(id_) + ")");
    const std::string days = std::to_string(daysAdmitted_) + " day(s)";
    bill.addItem("Doctor consultation", consultationFee);
    bill.addItem(roomName(room_) + " room x " + days, roomRate(room_) * daysAdmitted_);
    bill.addItem("Nursing care x " + days, NURSING_PER_DAY * daysAdmitted_);
    return bill;
}

void InPatient::displayExtra(std::ostream& os) const {
    os << " | Room: " << roomName(room_) << " x " << daysAdmitted_ << " day(s)";
}
