#pragma once
// Doctor IS-A Person (single inheritance).

#include <memory>
#include <string>
#include <vector>

#include "Person.h"

class Doctor : public Person {
private:
    std::string specialization_;
    double consultationFee_;

public:
    Doctor(int id, const std::string& name, int age, const std::string& phone,
           const std::string& specialization, double consultationFee);

    const std::string& getSpecialization() const { return specialization_; }
    double getFee() const { return consultationFee_; }
    void setFee(double fee);

    std::string getRole() const override { return "Doctor"; }
    std::string serialize() const override;
    void display(std::ostream& os) const override;

    // Builds a Doctor from one saved line's fields.
    static std::shared_ptr<Doctor> fromRecord(const std::vector<std::string>& fields);
};
