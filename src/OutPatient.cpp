#include "OutPatient.h"

#include <ostream>

#include "Exceptions.h"

OutPatient::OutPatient(int id, const std::string& name, int age, const std::string& phone,
                       const std::string& ailment)
    : Patient(id, name, age, phone, ailment) {}

void OutPatient::addLabTests(int count) {
    if (count < 1) throw ValidationException("number of lab tests must be at least 1");
    labTests_ += count;
}

void OutPatient::setCounts(int visits, int labTests) {
    if (visits < 0 || labTests < 0) throw ValidationException("counts cannot be negative");
    visits_ = visits;
    labTests_ = labTests;
}

std::string OutPatient::serialize() const {
    return serializeCommon() + '|' + std::to_string(visits_) + '|' + std::to_string(labTests_);
}

Bill OutPatient::generateBill(double consultationFee) const {
    Bill bill("Out-Patient " + name_ + " (ID " + std::to_string(id_) + ")");
    bill.addItem("Registration", REGISTRATION_FEE);
    if (visits_ > 0) {
        bill.addItem("Consultation x " + std::to_string(visits_) + " visit(s)",
                     consultationFee * visits_);
    }
    if (labTests_ > 0) {
        bill.addItem("Lab tests x " + std::to_string(labTests_), LAB_TEST_FEE * labTests_);
    }
    return bill;
}

void OutPatient::displayExtra(std::ostream& os) const {
    os << " | Visits: " << visits_ << " | Lab tests: " << labTests_;
}
