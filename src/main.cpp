#include "pricer/EuropeanCall.h"
#include "pricer/MonteCarloResult.h"
#include "pricer/Parameters.h"
#include <algorithm>
#include <cmath>

int num_paths = 1000000;

int main(){
    Parameters params(100.0,1000.0, 1.0, 0.05, 0.2);
    EuropeanCall call(params);
    double sum_payoffs=0.0;
    double sum_payoffs_squared=0.0;

    for(int i=0; i<num_paths; i++){
        sum_payoffs +=std::max(0.0, call.price() - params.K());
        sum_payoffs_squared += sum_payoffs * sum_payoffs;
    }
    double mean_payoffs= sum_payoffs / num_paths;
    double mean_payoffs_squared= sum_payoffs_squared / num_paths;
    double std_error = std::sqrt((mean_payoffs_squared - (mean_payoffs * mean_payoffs)) / num_paths);
    double discounted= std::exp(-params.r() * params.T());
    double option_price = discounted * mean_payoffs;
    struct MonteCarloResult result{mean_payoffs, std_error, num_paths};  
    return 0;
}