#pragma once
#include <nlohmann/json.hpp>
class Parameters {
public:
    Parameters() = default;
    Parameters(double S0, double K, double T, double r, double sigma, double num_paths = 1000000)
        : S0_(S0), K_(K), T_(T), r_(r), sigma_(sigma), num_paths_(num_paths) {}
    double S0() const { return S0_; }
    double K() const { return K_; }
    double T() const { return T_; }
    double r() const { return r_; }
    double sigma() const { return sigma_; }
    double num_paths() const { return num_paths_; }
private:    double S0_, K_, T_, r_, sigma_, num_paths_;
NLOHMANN_DEFINE_TYPE_INTRUSIVE(Parameters, S0_, K_, T_, r_, sigma_, num_paths_);
};
