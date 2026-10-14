#pragma once
// Admitted patient: billed per day for room and nursing.

#include "Patient.h"

enum class RoomType { General = 1, SemiPrivate = 2, Private = 3 };

class InPatient : public Patient {
private:
    RoomType room_;
    int daysAdmitted_;

protected:
    void displayExtra(std::ostream& os) const override;

public:
    static constexpr double NURSING_PER_DAY = 500.0;

    InPatient(int id, const std::string& name, int age, const std::string& phone,
              const std::string& ailment, RoomType room, int daysAdmitted);

    RoomType getRoom() const { return room_; }
    int getDays() const { return daysAdmitted_; }
    void extendStay(int extraDays);

    static double roomRate(RoomType room);
    static std::string roomName(RoomType room);
    static RoomType roomFromInt(int value);

    std::string getRole() const override { return "In-Patient"; }
    std::string getType() const override { return "IN"; }
    std::string serialize() const override;
    Bill generateBill(double consultationFee) const override;
};
