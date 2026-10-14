#pragma once
// Custom exception hierarchy for the Hospital Management System.
// Every domain error derives from HospitalException, so callers can catch
// the whole family with one handler or a specific error when it matters.

#include <stdexcept>
#include <string>

class HospitalException : public std::runtime_error {
public:
    explicit HospitalException(const std::string& message)
        : std::runtime_error(message) {}
};

// Thrown when an ID lookup fails (patient, doctor or appointment).
class NotFoundException : public HospitalException {
public:
    explicit NotFoundException(const std::string& message)
        : HospitalException("Not found: " + message) {}
};

// Thrown when input breaks a business rule (bad age, bad phone, etc.).
class ValidationException : public HospitalException {
public:
    explicit ValidationException(const std::string& message)
        : HospitalException("Invalid input: " + message) {}
};

// Thrown when a doctor or patient is already booked for a slot.
class SchedulingConflictException : public HospitalException {
public:
    explicit SchedulingConflictException(const std::string& message)
        : HospitalException("Scheduling conflict: " + message) {}
};
