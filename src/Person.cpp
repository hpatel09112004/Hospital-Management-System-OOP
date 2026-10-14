#include "Person.h"

#include <cctype>
#include <ostream>

#include "Exceptions.h"
#include "Utils.h"

int Person::population_ = 0;

Person::Person(int id, const std::string& name, int age, const std::string& phone)
    : id_(id), age_(0) {
    if (id <= 0) throw ValidationException("ID must be positive");
    setName(name);
    setAge(age);
    setPhone(phone);
    ++population_;
}

Person::~Person() { --population_; }

void Person::setName(const std::string& name) {
    util::requireClean(name, "Name");
    name_ = util::trim(name);
}

void Person::setAge(int age) {
    if (age < 0 || age > 130) throw ValidationException("age must be between 0 and 130");
    age_ = age;
}

void Person::setPhone(const std::string& phone) {
    const std::string p = util::trim(phone);
    if (p.size() != 10) throw ValidationException("phone number must have exactly 10 digits");
    for (char c : p) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw ValidationException("phone number must contain digits only");
        }
    }
    phone_ = p;
}

void Person::display(std::ostream& os) const {
    os << name_ << " (ID " << id_ << ") | " << getRole() << " | Age " << age_ << " | Ph "
       << phone_;
}

std::ostream& operator<<(std::ostream& os, const Person& person) {
    person.display(os);  // virtual call -> runtime polymorphism
    return os;
}
