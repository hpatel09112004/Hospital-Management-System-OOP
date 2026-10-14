#include "Patient.h"

#include <ostream>
#include <sstream>

#include "Exceptions.h"
#include "Utils.h"

Patient::Patient(int id, const std::string& name, int age, const std::string& phone,
                 const std::string& ailment)
    : Person(id, name, age, phone) {
    util::requireClean(ailment, "Ailment");
    ailment_ = util::trim(ailment);
}

void Patient::assignDoctor(int doctorId) {
    if (doctorId <= 0) throw ValidationException("doctor ID must be positive");
    doctorId_ = doctorId;
}

void Patient::addHistory(const std::string& entry) {
    const std::string clean = util::sanitize(entry);
    if (!clean.empty()) history_.push_back(clean);
}

std::string Patient::serializeCommon() const {
    std::ostringstream os;
    os << getType() << '|' << id_ << '|' << name_ << '|' << age_ << '|' << phone_ << '|'
       << ailment_ << '|' << doctorId_ << '|' << util::join(history_, '~');
    return os.str();
}

void Patient::display(std::ostream& os) const {
    Person::display(os);
    os << " | Ailment: " << ailment_ << " | Doctor: "
       << (hasDoctor() ? std::to_string(doctorId_) : std::string("not assigned"));
    displayExtra(os);
}
