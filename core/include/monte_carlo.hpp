#pragma once

#include <cstddef>
#include <vector>

#include "black_scholes.hpp"

namespace risk_engine::monte_carlo {

double price_european_option_mc(
    double spot,
    double strike,
    double maturity,
    double risk_free_rate,
    double volatility,
    double dividend_yield,
    bool is_call,
    std::size_t num_paths,
    unsigned int seed
);

std::vector<double> price_options_mc(
    const std::vector<black_scholes::OptionTrade>& trades,
    std::size_t num_paths,
    unsigned int seed
);

double calculate_portfolio_value_mc(
    const std::vector<black_scholes::OptionTrade>& trades,
    std::size_t num_paths,
    unsigned int seed
);

} // namespace risk_engine::monte_carlo