#pragma once
class Parameters {
public:
    Parameters(double S0, double K, double T, double r, double sigma)
        : S0_(S0), K_(K), T_(T), r_(r), sigma_(sigma) {}
    double S0() const { return S0_; }
    double K() const { return K_; }
    double T() const { return T_; }
    double r() const { return r_; }
    double sigma() const { return sigma_; }
private:    double S0_, K_, T_, r_, sigma_;
};