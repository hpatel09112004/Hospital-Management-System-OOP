#pragma once
// Factory pattern: callers ask for "a patient" and get the right subclass,
// returned through a base-class pointer.

#include <memory>
#include <string>
#include <vector>

#include "Patient.h"

class PatientFactory {
public:
    // Rebuilds an InPatient or OutPatient from one saved line's fields.
    static std::shared_ptr<Patient> fromRecord(const std::vector<std::string>& fields);
};
