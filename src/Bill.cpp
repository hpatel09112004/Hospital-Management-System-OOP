#include "Bill.h"

#include <iomanip>
#include <ostream>
#include <utility>

#include "Exceptions.h"
#include "Utils.h"

Bill::Bill(std::string title) : title_(std::move(title)) {}

void Bill::addItem(const std::string& description, double amount) {
    if (amount < 0) throw ValidationException("bill amounts cannot be negative");
    items_.push_back({description, amount});
}

void Bill::applyDiscount(double percent, const std::string& reason) {
    if (percent <= 0 || percent > 100) throw ValidationException("discount must be 1-100%");
    const double discount = total() * percent / 100.0;
    // Stored as a negative line so the printout shows it explicitly.
    items_.push_back({reason + " (" + std::to_string(static_cast<int>(percent)) + "%)",
                      -discount});
}

double Bill::total() const {
    double sum = 0;
    for (const auto& item : items_) sum += item.amount;
    return sum;
}

Bill& Bill::operator+=(const LineItem& item) {
    addItem(item.description, item.amount);
    return *this;
}

Bill Bill::operator+(const Bill& other) const {
    Bill merged(title_ + " + " + other.title_);
    merged.items_ = items_;
    merged.items_.insert(merged.items_.end(), other.items_.begin(), other.items_.end());
    return merged;
}

std::ostream& operator<<(std::ostream& os, const Bill& bill) {
    const std::string rule(52, '-');
    os << rule << '\n' << " BILL: " << bill.title_ << '\n' << rule << '\n';
    for (const auto& item : bill.items_) {
        os << ' ' << std::left << std::setw(36) << item.description << std::right
           << std::setw(14) << util::money(item.amount) << '\n';
    }
    os << rule << '\n'
       << ' ' << std::left << std::setw(36) << "TOTAL" << std::right << std::setw(14)
       << util::money(bill.total()) << '\n'
       << rule << '\n';
    return os;
}
