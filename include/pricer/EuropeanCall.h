#pragma once
#include "Parameters.h"

class EuropeanCall {
public:
    EuropeanCall(const Parameters& params) : params_(params) {}
    double price() ;
    double getStdError() const { return std_error; }
    double getMeanPayoffs() const { return mean_payoffs; }
private:    Parameters params_;
            double std_error;
            double mean_payoffs;
};