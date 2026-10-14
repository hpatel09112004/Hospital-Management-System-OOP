#pragma once
// Walk-in patient: billed per consultation visit plus lab tests.

#include "Patient.h"

class OutPatient : public Patient {
private:
    int visits_ = 0;
    int labTests_ = 0;

protected:
    void displayExtra(std::ostream& os) const override;

public:
    static constexpr double REGISTRATION_FEE = 100.0;
    static constexpr double LAB_TEST_FEE = 350.0;

    OutPatient(int id, const std::string& name, int age, const std::string& phone,
               const std::string& ailment);

    int getVisits() const { return visits_; }
    int getLabTests() const { return labTests_; }
    void addLabTests(int count);
    void setCounts(int visits, int labTests);  // used when loading from file

    void recordVisit() override { ++visits_; }

    std::string getRole() const override { return "Out-Patient"; }
    std::string getType() const override { return "OUT"; }
    std::string serialize() const override;
    Bill generateBill(double consultationFee) const override;
};
