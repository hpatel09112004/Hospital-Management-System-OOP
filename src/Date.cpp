#include "Date.h"

#include <iomanip>
#include <sstream>
#include <tuple>

#include "Exceptions.h"

Date::Date(int day, int month, int year) : day_(day), month_(month), year_(year) {
    if (!isValid(day, month, year)) {
        throw ValidationException("date " + std::to_string(day) + "-" +
                                  std::to_string(month) + "-" + std::to_string(year) +
                                  " does not exist");
    }
}

bool Date::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

bool Date::isValid(int day, int month, int year) {
    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1) return false;
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    if (month == 2 && isLeapYear(year)) maxDay = 29;
    return day <= maxDay;
}

Date Date::parse(const std::string& text) {
    std::istringstream in(text);
    int d = 0, m = 0, y = 0;
    char sep1 = 0, sep2 = 0;
    if (!(in >> d >> sep1 >> m >> sep2 >> y) || sep1 != '-' || sep2 != '-') {
        throw ValidationException("date must be in DD-MM-YYYY format, got '" + text + "'");
    }
    std::string rest;
    if (in >> rest) {
        throw ValidationException("unexpected text after date: '" + text + "'");
    }
    return Date(d, m, y);
}

std::string Date::toString() const {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(2) << day_ << '-' << std::setw(2) << month_ << '-'
       << std::setw(4) << year_;
    return os.str();
}

bool Date::operator<(const Date& other) const {
    return std::tie(year_, month_, day_) < std::tie(other.year_, other.month_, other.day_);
}

bool Date::operator==(const Date& other) const {
    return day_ == other.day_ && month_ == other.month_ && year_ == other.year_;
}

std::ostream& operator<<(std::ostream& os, const Date& date) {
    return os << date.toString();
}
