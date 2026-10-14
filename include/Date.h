#pragma once
// Value class for a calendar date (DD-MM-YYYY) with validation and
// overloaded comparison / stream operators.

#include <iosfwd>
#include <string>

class Date {
private:
    int day_;
    int month_;
    int year_;

public:
    Date(int day, int month, int year);  // throws ValidationException

    static Date parse(const std::string& text);  // "DD-MM-YYYY"
    static bool isValid(int day, int month, int year);
    static bool isLeapYear(int year);

    int day() const { return day_; }
    int month() const { return month_; }
    int year() const { return year_; }

    std::string toString() const;

    bool operator<(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const Date& date);
};
