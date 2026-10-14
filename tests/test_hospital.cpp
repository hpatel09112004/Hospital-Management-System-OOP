// Minimal self-contained unit tests (no external framework needed).
// Build and run with:  make test

#include <filesystem>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Exceptions.h"
#include "Hospital.h"

namespace {

int passed = 0;
int failed = 0;

void check(bool condition, const std::string& name) {
    if (condition) {
        ++passed;
        std::cout << "  [PASS] " << name << '\n';
    } else {
        ++failed;
        std::cout << "  [FAIL] " << name << '\n';
    }
}

template <typename Ex>
void checkThrows(const std::function<void()>& action, const std::string& name) {
    try {
        action();
        check(false, name + " (no exception)");
    } catch (const Ex&) {
        check(true, name);
    } catch (...) {
        check(false, name + " (wrong exception type)");
    }
}

bool near(double a, double b) { return a - b < 0.001 && b - a < 0.001; }

}  // namespace

int main() {
    std::cout << "Running Hospital Management System tests\n";

    // ---- Date ----
    check(Date::isValid(29, 2, 2028), "29 Feb in a leap year is valid");
    check(!Date::isValid(29, 2, 2026), "29 Feb in a non-leap year is invalid");
    check(Date(1, 1, 2026) < Date(2, 1, 2026), "Date operator< compares chronologically");
    check(Date::parse("05-11-2026").toString() == "05-11-2026", "Date parse/format round-trip");
    checkThrows<ValidationException>([] { Date::parse("2026/11/05"); }, "Bad date format rejected");

    // ---- Validation / encapsulation ----
    Hospital h("Test Hospital");
    checkThrows<ValidationException>([&] { h.addDoctor("A", 30, "123", "X", 100); },
                                     "Short phone number rejected");
    checkThrows<ValidationException>([&] { h.addDoctor("A", 18, "9000000000", "X", 100); },
                                     "Doctor under 21 rejected");
    checkThrows<ValidationException>([&] { h.registerOutPatient("A|B", 30, "9000000000", "X"); },
                                     "Reserved character in name rejected");

    auto doc = h.addDoctor("Meera", 40, "9000000001", "Cardiology", 1000);
    auto doc2 = h.addDoctor("Ravi", 50, "9000000002", "Neurology", 800);
    check(doc->getId() == 101 && doc2->getId() == 102, "Doctor IDs auto-increment from 101");

    // ---- Polymorphic billing ----
    auto in = h.admitInPatient("Kiran", 35, "9000000003", "Fracture", RoomType::Private, 3);
    auto out = h.registerOutPatient("Sana", 25, "9000000004", "Fever");
    h.assignDoctor(in->getId(), doc->getId());
    h.assignDoctor(out->getId(), doc2->getId());

    // In: 1000 consult + 3*5000 room + 3*500 nursing = 17500
    check(near(h.generateBill(in->getId()).total(), 17500.0), "In-patient bill = 17500");
    // Out with no visits yet: only the 100 registration fee
    check(near(h.generateBill(out->getId()).total(), 100.0), "Out-patient bill = registration only");

    Patient* base = out.get();  // base-class pointer
    check(base->getRole() == "Out-Patient", "Virtual getRole() dispatches to subclass");

    // ---- Appointments & conflicts ----
    auto a1 = h.bookAppointment(out->getId(), doc2->getId(), Date(10, 11, 2026), 10);
    checkThrows<SchedulingConflictException>(
        [&] { h.bookAppointment(in->getId(), doc2->getId(), Date(10, 11, 2026), 10); },
        "Double-booking a doctor is blocked");
    checkThrows<SchedulingConflictException>(
        [&] { h.bookAppointment(out->getId(), doc->getId(), Date(10, 11, 2026), 10); },
        "Double-booking a patient is blocked");
    checkThrows<ValidationException>(
        [&] { h.bookAppointment(out->getId(), doc->getId(), Date(10, 11, 2026), 20); },
        "Slot outside 9-16 rejected");

    h.completeAppointment(a1->getId(), "Viral fever");
    h.orderLabTests(out->getId(), 2);
    // 100 + 800*1 + 350*2 = 1600
    check(near(h.generateBill(out->getId()).total(), 1600.0),
          "Out-patient bill after 1 visit + 2 labs = 1600");
    check(out->getHistory().size() == 1, "Completed visit added to history");
    checkThrows<ValidationException>([&] { h.completeAppointment(a1->getId(), ""); },
                                     "Completing twice is rejected");
    checkThrows<ValidationException>([&] { h.orderLabTests(in->getId(), 1); },
                                     "dynamic_cast blocks lab tests for in-patient");

    // ---- Senior discount ----
    auto senior = h.registerOutPatient("Gopal", 72, "9000000005", "Checkup");
    h.assignDoctor(senior->getId(), doc->getId());
    check(near(h.generateBill(senior->getId()).total(), 90.0), "Senior gets 10% discount");

    // ---- Operator overloading ----
    Bill b1("A");
    b1 += LineItem{"x", 100};
    Bill b2("B");
    b2.addItem("y", 50);
    const Bill merged = b1 + b2;
    check(near(merged.total(), 150.0) && merged.itemCount() == 2, "Bill operator+ merges items");

    // ---- Repository template ----
    checkThrows<NotFoundException>([&] { h.getPatient(9999); }, "Unknown patient throws NotFound");
    check(h.searchDoctors("cardio").size() == 1, "Case-insensitive specialization search");

    // ---- Persistence round-trip ----
    const std::string dir = "build/test_data";
    std::filesystem::remove_all(dir);
    h.saveToDirectory(dir);
    Hospital copy("Copy");
    std::ostringstream warnings;
    const int count = copy.loadFromDirectory(dir, warnings);
    // 2 doctors + 3 patients + 1 appointment
    check(count == 6 && warnings.str().empty(), "Save/load restores all 6 records");
    check(near(copy.generateBill(out->getId()).total(), 1600.0), "Reloaded bill matches original");
    auto newDoc = copy.addDoctor("New", 45, "9000000006", "ENT", 600);
    check(newDoc->getId() == 103, "IDs continue after reload");

    // ---- Discharge ----
    h.dischargePatient(in->getId());
    checkThrows<NotFoundException>([&] { h.getPatient(in->getId()); },
                                   "Discharged patient removed");

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
