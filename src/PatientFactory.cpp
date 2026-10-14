#include "PatientFactory.h"

#include "Exceptions.h"
#include "InPatient.h"
#include "OutPatient.h"
#include "Utils.h"

// Record layout:
//   IN |id|name|age|phone|ailment|doctorId|history|room|days
//   OUT|id|name|age|phone|ailment|doctorId|history|visits|labTests
std::shared_ptr<Patient> PatientFactory::fromRecord(const std::vector<std::string>& f) {
    if (f.size() != 10) throw ValidationException("patient record needs 10 fields");

    const int id = std::stoi(f[1]);
    const int age = std::stoi(f[3]);
    std::shared_ptr<Patient> patient;

    if (f[0] == "IN") {
        patient = std::make_shared<InPatient>(id, f[2], age, f[4], f[5],
                                              InPatient::roomFromInt(std::stoi(f[8])),
                                              std::stoi(f[9]));
    } else if (f[0] == "OUT") {
        auto out = std::make_shared<OutPatient>(id, f[2], age, f[4], f[5]);
        out->setCounts(std::stoi(f[8]), std::stoi(f[9]));
        patient = out;
    } else {
        throw ValidationException("unknown patient type '" + f[0] + "'");
    }

    const int doctorId = std::stoi(f[6]);
    if (doctorId > 0) patient->assignDoctor(doctorId);
    for (const auto& entry : util::split(f[7], '~')) {
        if (!entry.empty()) patient->addHistory(entry);
    }
    return patient;
}
