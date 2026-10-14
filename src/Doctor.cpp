#include "Doctor.h"

#include <ostream>
#include <sstream>

#include "Exceptions.h"
#include "Utils.h"

Doctor::Doctor(int id, const std::string& name, int age, const std::string& phone,
               const std::string& specialization, double consultationFee)
    : Person(id, name, age, phone), consultationFee_(0) {
    if (age < 21) throw ValidationException("a doctor must be at least 21 years old");
    util::requireClean(specialization, "Specialization");
    specialization_ = util::trim(specialization);
    setFee(consultationFee);
}

void Doctor::setFee(double fee) {
    if (fee <= 0) throw ValidationException("consultation fee must be positive");
    consultationFee_ = fee;
}

std::string Doctor::serialize() const {
    std::ostringstream os;
    os << id_ << '|' << name_ << '|' << age_ << '|' << phone_ << '|' << specialization_ << '|'
       << consultationFee_;
    return os.str();
}

void Doctor::display(std::ostream& os) const {
    os << "Dr. ";
    Person::display(os);  // reuse the base-class part
    os << " | " << specialization_ << " | Fee " << util::money(consultationFee_);
}

std::shared_ptr<Doctor> Doctor::fromRecord(const std::vector<std::string>& f) {
    if (f.size() != 6) throw ValidationException("doctor record needs 6 fields");
    return std::make_shared<Doctor>(std::stoi(f[0]), f[1], std::stoi(f[2]), f[3], f[4],
                                    std::stod(f[5]));
}
