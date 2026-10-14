#pragma once
// Patient IS-A Person and IS Billable (multiple inheritance).
// Still abstract: InPatient and OutPatient decide how billing works.
//
//      Person      Billable
//          |        |
//          +-- Patient --+
//          |             |
//      InPatient     OutPatient      (multilevel + hierarchical)

#include <string>
#include <vector>

#include "Billable.h"
#include "Person.h"

class Patient : public Person, public Billable {
protected:
    std::string ailment_;
    int doctorId_ = 0;  // 0 = no doctor assigned yet
    std::vector<std::string> history_;

    // Template-method hook: subclasses append their own details.
    virtual void displayExtra(std::ostream& os) const = 0;
    // Fields every patient record shares (used by subclasses' serialize()).
    std::string serializeCommon() const;

public:
    Patient(int id, const std::string& name, int age, const std::string& phone,
            const std::string& ailment);

    const std::string& getAilment() const { return ailment_; }
    int getDoctorId() const { return doctorId_; }
    bool hasDoctor() const { return doctorId_ != 0; }
    void assignDoctor(int doctorId);

    void addHistory(const std::string& entry);
    const std::vector<std::string>& getHistory() const { return history_; }

    virtual std::string getType() const = 0;  // "IN" or "OUT"
    virtual void recordVisit() {}             // default: nothing to count

    void display(std::ostream& os) const override;
};
