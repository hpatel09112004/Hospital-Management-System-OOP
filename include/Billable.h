#pragma once
// Pure interface (abstract class with only pure virtual functions).
// Anything that can be billed implements this, independent of the Person tree.

#include "Bill.h"

class Billable {
public:
    virtual ~Billable() = default;
    virtual Bill generateBill(double consultationFee) const = 0;
};
