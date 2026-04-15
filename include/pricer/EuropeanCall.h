#pragma once
#include "Parameters.h"

class EuropeanCall {
public:
    EuropeanCall(const Parameters& params) : params_(params) {}
    double price() const;
private:    Parameters params_;
};