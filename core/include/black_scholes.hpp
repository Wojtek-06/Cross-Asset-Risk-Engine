/* This file contains the */

#pragma once

#include <vector>

namespace risk_engine::black_scholes {

struct OptionGreeks {
    double delta;
    double gamma;
    double vega_1bp;
    double theta;
};

struct OptionTrade {
    double spot;
    double strike;
    double maturity;
    double risk_free_rate;
    double volatility;
    double dividend_yield;
    bool is_call;
    double quantity;
};

double calculate_price(
    double spot,
    double strike,
    double maturity,
    double risk_free_rate,
    double volatility,
    double dividend_yield,
    bool is_call
);

std::vector<double> calculate_prices(
    const std::vector<OptionTrade>& trades
);

OptionGreeks calculate_greeks(
    double spot,
    double strike,
    double maturity,
    double risk_free_rate,
    double volatility,
    double dividend_yield,
    bool is_call
);

struct PortfolioGreeks {
    double delta;
    double gamma;
    double vega_1bp;
    double theta;
};

std::vector<OptionGreeks> calculate_greeks(
    const std::vector<OptionTrade>& trades
); 

PortfolioGreeks aggregate_greeks(
    const std::vector<OptionGreeks>& greeks
);

PortfolioGreeks calculate_portfolio_greeks(
    const std::vector<OptionTrade>& trades
);

double calculate_portfolio_value(
    const std::vector<OptionTrade>& trades
);

double calculate_portfolio_value_mc(
    const std::vector<black_scholes::OptionTrade>& trades,
    std::size_t num_paths,
    unsigned int seed
);

} // namespace risk_engine::black_scholes