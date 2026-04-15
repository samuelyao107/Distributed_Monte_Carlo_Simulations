#include <nlohmann/json.hpp>

struct MonteCarloResult {
    double price;
    double std_error;
    int    num_paths;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MonteCarloResult, price, std_error, num_paths)