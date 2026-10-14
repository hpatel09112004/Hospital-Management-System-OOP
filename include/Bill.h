#pragma once
// A bill made of line items. Shows operator overloading:
//   bill += item     add a line
//   billA + billB    merge two bills
//   std::cout << bill   formatted printout

#include <iosfwd>
#include <string>
#include <vector>

struct LineItem {
    std::string description;
    double amount;
};

class Bill {
private:
    std::string title_;
    std::vector<LineItem> items_;

public:
    explicit Bill(std::string title = "Bill");

    void addItem(const std::string& description, double amount);
    void applyDiscount(double percent, const std::string& reason);

    double total() const;
    std::size_t itemCount() const { return items_.size(); }
    const std::vector<LineItem>& items() const { return items_; }
    const std::string& title() const { return title_; }

    Bill& operator+=(const LineItem& item);
    Bill operator+(const Bill& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Bill& bill);
};
