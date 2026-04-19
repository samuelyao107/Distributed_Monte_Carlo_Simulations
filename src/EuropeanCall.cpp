#include "pricer/EuropeanCall.h"
#include <cmath>
#include <random>

double EuropeanCall::price() {
    
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> normal(0.0, 1.0);
    double Z = 0.0;
    double payoff = 0.0;
    double sum_payoffs=0.0;
    double sum_payoffs_squared=0.0;

    for(int i=0; i<this->params_.num_paths(); i++){
        Z = normal(rng);
        payoff =std::max(0.0, (this->params_.S0() * std::exp((this->params_.r() - 0.5 * std::pow(this->params_.sigma(), 2)) * this->params_.T() + this->params_.sigma() * std::sqrt(this->params_.T())* Z)) - this->params_.K());
        sum_payoffs += payoff;
        sum_payoffs_squared += payoff * payoff;
    }
    double mean_payoffs= sum_payoffs / this->params_.num_paths();
    double mean_payoffs_squared= sum_payoffs_squared / this->params_.num_paths();
    double std_error = std::sqrt((mean_payoffs_squared - (mean_payoffs * mean_payoffs)) / this->params_.num_paths());
    this->std_error = std_error;
    this->mean_payoffs = mean_payoffs;
    double discounted= std::exp(-this->params_.r() * this->params_.T());
    double option_price = discounted * mean_payoffs;
    return option_price;

}

