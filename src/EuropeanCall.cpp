#include "pricer/EuropeanCall.h"
#include <cmath>
#include <random>

double EuropeanCall::price() const {
    
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> normal(0.0, 1.0);
    double Z = normal(rng);
    return this->params_.S0() * std::exp((this->params_.r() - 0.5 * std::pow(this->params_.sigma(), 2)) * this->params_.T() + this->params_.sigma() * std::sqrt(this->params_.T())* Z); 
}