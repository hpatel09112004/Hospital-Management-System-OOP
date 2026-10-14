// Hospital Management System - console front end.
//
//   ./hms              interactive menu (data saved in ./data)
//   ./hms --demo       scripted walkthrough of every feature
//   ./hms --data DIR   use a different data folder

#include <iostream>
#include <limits>
#include <string>

#include "Exceptions.h"
#include "Hospital.h"
#include "Utils.h"

namespace {

const std::string HOSPITAL_NAME = "CareWell Multi-Speciality Hospital";

// Thrown when stdin closes (Ctrl+D / end of piped input) so we can save and exit cleanly.
struct InputClosed {};

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw InputClosed{};
    return util::trim(line);
}

std::string readText(const std::string& prompt) {
    while (true) {
        std::string value = readLine(prompt);
        if (!value.empty()) return value;
        std::cout << "  Please enter a value.\n";
    }
}

int readInt(const std::string& prompt, int low, int high) {
    while (true) {
        const std::string text = readLine(prompt);
        try {
            std::size_t used = 0;
            const int value = std::stoi(text, &used);
            if (used == text.size() && value >= low && value <= high) return value;
        } catch (const std::exception&) {
        }
        std::cout << "  Enter a whole number from " << low << " to " << high << ".\n";
    }
}

double readMoney(const std::string& prompt) {
    while (true) {
        const std::string text = readLine(prompt);
        try {
            std::size_t used = 0;
            const double value = std::stod(text, &used);
            if (used == text.size() && value > 0) return value;
        } catch (const std::exception&) {
        }
        std::cout << "  Enter a positive amount, e.g. 750.\n";
    }
}

// Validates while typing, so a typo doesn't throw away the rest of the form.
std::string readPhone(const std::string& prompt) {
    while (true) {
        const std::string phone = readLine(prompt);
        bool ok = phone.size() == 10;
        for (char c : phone) ok = ok && c >= '0' && c <= '9';
        if (ok) return phone;
        std::cout << "  Phone number must be exactly 10 digits.\n";
    }
}

Date readDate(const std::string& prompt) {
    while (true) {
        try {
            return Date::parse(readLine(prompt));
        } catch (const ValidationException& e) {
            std::cout << "  " << e.what() << '\n';
        }
    }
}

void heading(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

void showMenu() {
    std::cout << "\n+--------------------------------------------------+\n"
              << "|  " << HOSPITAL_NAME << "              |\n"
              << "+--------------------------------------------------+\n"
              << " Registration       1. Add doctor\n"
              << "                    2. Register out-patient\n"
              << "                    3. Admit in-patient\n"
              << "                    4. Assign doctor to patient\n"
              << " Appointments       5. Book appointment\n"
              << "                    6. Complete appointment\n"
              << "                    7. Cancel appointment\n"
              << " Treatment          8. Order lab tests (out-patient)\n"
              << "                    9. Extend stay (in-patient)\n"
              << " Billing           10. Generate bill\n"
              << "                   11. Discharge patient\n"
              << " Reports           12. List doctors\n"
              << "                   13. List patients\n"
              << "                   14. List appointments\n"
              << "                   15. Search doctors by specialization\n"
              << "                   16. View patient history\n"
              << "                   17. Statistics\n"
              << " Data              18. Save now\n"
              << "                    0. Save and exit\n";
}

// One menu action. Any HospitalException bubbles up to the caller's handler.
void handleChoice(Hospital& h, int choice, const std::string& dataDir) {
    switch (choice) {
        case 1: {
            heading("Add doctor");
            const std::string name = readText("  Name           : ");
            const int age = readInt("  Age            : ", 21, 100);
            const std::string phone = readPhone("  Phone (10 dig.): ");
            const std::string spec = readText("  Specialization : ");
            const double fee = readMoney("  Consultation fee (Rs.): ");
            auto d = h.addDoctor(name, age, phone, spec, fee);
            std::cout << "  Added: " << *d << '\n';
            break;
        }
        case 2: {
            heading("Register out-patient");
            const std::string name = readText("  Name    : ");
            const int age = readInt("  Age     : ", 0, 130);
            const std::string phone = readPhone("  Phone   : ");
            const std::string ailment = readText("  Ailment : ");
            auto p = h.registerOutPatient(name, age, phone, ailment);
            std::cout << "  Registered: " << *p << '\n';
            break;
        }
        case 3: {
            heading("Admit in-patient");
            const std::string name = readText("  Name    : ");
            const int age = readInt("  Age     : ", 0, 130);
            const std::string phone = readPhone("  Phone   : ");
            const std::string ailment = readText("  Ailment : ");
            std::cout << "  Room types: 1) General Rs.1500/day  2) Semi-Private Rs.3000/day"
                         "  3) Private Rs.5000/day\n";
            const RoomType room = InPatient::roomFromInt(readInt("  Room    : ", 1, 3));
            const int days = readInt("  Expected days: ", 1, 365);
            auto p = h.admitInPatient(name, age, phone, ailment, room, days);
            std::cout << "  Admitted: " << *p << '\n';
            break;
        }
        case 4: {
            heading("Assign doctor");
            const int pid = readInt("  Patient ID: ", 1, 999999);
            const int did = readInt("  Doctor ID : ", 1, 999999);
            h.assignDoctor(pid, did);
            std::cout << "  " << h.getPatient(pid)->getName() << " is now under Dr. "
                      << h.getDoctor(did)->getName() << ".\n";
            break;
        }
        case 5: {
            heading("Book appointment");
            const int pid = readInt("  Patient ID: ", 1, 999999);
            const int did = readInt("  Doctor ID : ", 1, 999999);
            const Date date = readDate("  Date (DD-MM-YYYY): ");
            const int slot = readInt("  Start hour (9-16): ", Appointment::FIRST_SLOT,
                                     Appointment::LAST_SLOT);
            auto a = h.bookAppointment(pid, did, date, slot);
            std::cout << "  Booked: " << *a << '\n';
            break;
        }
        case 6: {
            heading("Complete appointment");
            const int aid = readInt("  Appointment ID: ", 1, 999999);
            const std::string notes = readLine("  Doctor's notes (optional): ");
            h.completeAppointment(aid, notes);
            std::cout << "  Appointment #" << aid << " completed and added to history.\n";
            break;
        }
        case 7: {
            heading("Cancel appointment");
            const int aid = readInt("  Appointment ID: ", 1, 999999);
            h.cancelAppointment(aid);
            std::cout << "  Appointment #" << aid << " cancelled.\n";
            break;
        }
        case 8: {
            heading("Order lab tests");
            const int pid = readInt("  Patient ID: ", 1, 999999);
            const int count = readInt("  Number of tests: ", 1, 50);
            h.orderLabTests(pid, count);
            std::cout << "  " << count << " test(s) added.\n";
            break;
        }
        case 9: {
            heading("Extend stay");
            const int pid = readInt("  Patient ID: ", 1, 999999);
            const int days = readInt("  Extra days: ", 1, 365);
            h.extendStay(pid, days);
            std::cout << "  Stay extended: " << *h.getPatient(pid) << '\n';
            break;
        }
        case 10: {
            heading("Generate bill");
            std::cout << h.generateBill(readInt("  Patient ID: ", 1, 999999));
            break;
        }
        case 11: {
            heading("Discharge patient");
            const int pid = readInt("  Patient ID: ", 1, 999999);
            const Bill bill = h.dischargePatient(pid);
            std::cout << bill << "  Patient discharged.\n";
            break;
        }
        case 12: heading("Doctors"); h.printDoctors(std::cout); break;
        case 13: heading("Patients"); h.printPatients(std::cout); break;
        case 14: {
            heading("Appointments");
            const int did = readInt("  Doctor ID (0 for all): ", 0, 999999);
            h.printAppointments(std::cout, did);
            break;
        }
        case 15: {
            heading("Search doctors");
            const auto found = h.searchDoctors(readText("  Specialization contains: "));
            if (found.empty()) std::cout << "  No matching doctors.\n";
            for (const auto& d : found) std::cout << "  " << *d << '\n';
            break;
        }
        case 16:
            heading("Patient history");
            h.printHistory(std::cout, readInt("  Patient ID: ", 1, 999999));
            break;
        case 17: heading("Statistics"); h.printStats(std::cout); break;
        case 18:
            h.saveToDirectory(dataDir);
            std::cout << "  Saved to '" << dataDir << "'.\n";
            break;
        default: break;
    }
}

int runInteractive(const std::string& dataDir) {
    Hospital hospital(HOSPITAL_NAME);
    const int loaded = hospital.loadFromDirectory(dataDir, std::cerr);
    std::cout << "Loaded " << loaded << " record(s) from '" << dataDir << "'.\n";

    try {
        while (true) {
            showMenu();
            const int choice = readInt("Choose an option: ", 0, 18);
            if (choice == 0) break;
            try {
                handleChoice(hospital, choice, dataDir);
            } catch (const HospitalException& e) {
                std::cout << "  ! " << e.what() << '\n';  // one handler for the whole family
            }
        }
    } catch (const InputClosed&) {
        std::cout << "\nInput closed.\n";
    }

    hospital.saveToDirectory(dataDir);
    std::cout << "Data saved to '" << dataDir << "'. Goodbye!\n";
    return 0;
}

// Scripted run that exercises every feature; also used by CI as a smoke test.
int runDemo() {
    const std::string demoDir = "demo_data";
    Hospital h(HOSPITAL_NAME);

    heading("1. Registering doctors");
    auto cardio = h.addDoctor("Anita Rao", 48, "9840012345", "Cardiology", 900);
    auto ortho = h.addDoctor("Vikram Menon", 41, "9840023456", "Orthopaedics", 750);
    auto general = h.addDoctor("Farah Khan", 35, "9840034567", "General Medicine", 500);
    h.printDoctors(std::cout);

    heading("2. Registering patients (Factory-style subclasses behind Patient*)");
    auto p1 = h.registerOutPatient("Rohan Das", 21, "9790011111", "Recurring migraine");
    auto p2 = h.admitInPatient("Lakshmi Iyer", 67, "9790022222", "Hip fracture",
                               RoomType::SemiPrivate, 4);
    auto p3 = h.registerOutPatient("Arjun Nair", 29, "9790033333", "Chest pain on exertion");
    h.printPatients(std::cout);

    heading("3. Booking appointments");
    h.bookAppointment(p1->getId(), general->getId(), Date(15, 10, 2026), 10);
    h.bookAppointment(p3->getId(), cardio->getId(), Date(15, 10, 2026), 11);
    h.bookAppointment(p1->getId(), general->getId(), Date(22, 10, 2026), 10);
    h.assignDoctor(p2->getId(), ortho->getId());
    h.printAppointments(std::cout);

    heading("4. Exception handling");
    try {
        h.bookAppointment(p3->getId(), general->getId(), Date(15, 10, 2026), 10);
    } catch (const SchedulingConflictException& e) {
        std::cout << "  Caught SchedulingConflictException -> " << e.what() << '\n';
    }
    try {
        h.getPatient(4242);
    } catch (const NotFoundException& e) {
        std::cout << "  Caught NotFoundException          -> " << e.what() << '\n';
    }
    try {
        h.addDoctor("Test", 30, "12345", "Neurology", 600);
    } catch (const ValidationException& e) {
        std::cout << "  Caught ValidationException        -> " << e.what() << '\n';
    }
    try {
        h.orderLabTests(p2->getId(), 2);
    } catch (const HospitalException& e) {
        std::cout << "  Caught via base HospitalException -> " << e.what() << '\n';
    }

    heading("5. Completing visits, lab tests and stay extension");
    h.completeAppointment(1, "Prescribed rest and hydration");
    h.completeAppointment(2, "ECG normal; stress test advised");
    h.orderLabTests(p3->getId(), 2);
    h.extendStay(p2->getId(), 1);
    h.printHistory(std::cout, p1->getId());
    h.printHistory(std::cout, p3->getId());

    heading("6. Polymorphic billing (same call, different behaviour)");
    for (int id : {p1->getId(), p2->getId(), p3->getId()}) {
        std::cout << h.generateBill(id) << '\n';
    }

    heading("7. Operator overloading on Bill (+ and +=)");
    Bill pharmacy("Pharmacy");
    pharmacy += LineItem{"Paracetamol 650mg x 10", 32.0};
    pharmacy += LineItem{"Sumatriptan 50mg x 6", 210.0};
    std::cout << h.generateBill(p1->getId()) + pharmacy;

    heading("8. Discharging an in-patient");
    std::cout << h.dischargePatient(p2->getId()) << "  Lakshmi Iyer discharged.\n";

    heading("9. Statistics");
    h.printStats(std::cout);

    heading("10. File persistence round-trip");
    h.saveToDirectory(demoDir);
    Hospital reloaded(HOSPITAL_NAME);
    const int count = reloaded.loadFromDirectory(demoDir, std::cerr);
    std::cout << "  Saved to '" << demoDir << "' and reloaded " << count
              << " record(s) into a fresh Hospital object:\n";
    reloaded.printPatients(std::cout);
    reloaded.printAppointments(std::cout);

    std::cout << "\nDemo finished successfully.\n";
    return 0;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string dataDir = "data";
    bool demo = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--demo") {
            demo = true;
        } else if (arg == "--data" && i + 1 < argc) {
            dataDir = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: hms [--demo] [--data DIR]\n";
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\nUsage: hms [--demo] [--data DIR]\n";
            return 1;
        }
    }

    try {
        return demo ? runDemo() : runInteractive(dataDir);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
}
