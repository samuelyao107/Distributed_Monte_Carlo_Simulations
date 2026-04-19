#include <gtest/gtest.h>
#include <cmath>
#include "pricer/EuropeanCall.h"
#include "pricer/Parameters.h"
#include "pricer/MonteCarloResult.h"

// --- La référence analytique Black-Scholes ---
double black_scholes_call(double S0, double K, double r,
                           double sigma, double T) {
    double d1 = (std::log(S0 / K) + (r + 0.5 * sigma * sigma) * T)
                / (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);

    auto N = [](double x) {
        return 0.5 * std::erfc(-x / std::sqrt(2.0));
    };

    return S0 * N(d1) - K * std::exp(-r * T) * N(d2);
}

// --- Le test ---
TEST(EuropeanCallTest, MatchesBlackScholes) {
    double S0    = 100.0;
    double K     = 100.0;
    double r     = 0.05;
    double sigma = 0.20;
    double T     = 1.0;
    int    N     = 1000000;
    Parameters params(S0, K, T, r, sigma);
    EuropeanCall call(params);
    double result = call.price();

    double bs_price = black_scholes_call(S0, K, r, sigma, T);

    EXPECT_NEAR(result, bs_price, 0.05);
}