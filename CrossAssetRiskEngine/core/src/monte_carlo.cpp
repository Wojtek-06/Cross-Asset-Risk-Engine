#include "monte_carlo.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace risk_engine::monte_carlo {

    namespace {

        void validate_inputs(
            double spot,
            double strike,
            double maturity,
            double volatility,
            std::size_t num_paths
        ) {
            if (spot <= 0.0) {
                throw std::invalid_argument("Spot price must be greater than zero.");
            }
        
            if (strike <= 0.0) {
                throw std::invalid_argument("Strike price must be greater than zero.");
            }
        
            if (maturity <= 0.0) {
                throw std::invalid_argument("Maturity must be greater than zero.");
            }
        
            if (volatility <= 0.0) {
                throw std::invalid_argument("Volatility must be greater than zero.");
            }
        
            if (num_paths == 0) {
                throw std::invalid_argument("Number of Monte Carlo paths must be greater than zero.");
            }
        }
        
        } // namespace

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
        ) {
            validate_inputs(spot, strike, maturity, volatility, num_paths);
        
            std::mt19937 generator(seed);
            std::normal_distribution<double> normal(0.0, 1.0);

    const double drift =
        (risk_free_rate - dividend_yield - 0.5 * volatility * volatility) * maturity;

    const double diffusion_scale =
        volatility * std::sqrt(maturity);

    double payoff_sum = 0.0;

    for (std::size_t i = 0; i < num_paths; ++i) {
        const double z = normal(generator);

        const double terminal_spot =
            spot * std::exp(drift + diffusion_scale * z);

        const double payoff = is_call
            ? std::max(terminal_spot - strike, 0.0)
            : std::max(strike - terminal_spot, 0.0);

        payoff_sum += payoff;
    }

    const double average_payoff =
        payoff_sum / static_cast<double>(num_paths);

    return std::exp(-risk_free_rate * maturity) * average_payoff;
}

std::vector<double> price_options_mc(
    const std::vector<black_scholes::OptionTrade>& trades,
    std::size_t num_paths,
    unsigned int seed
) {
    std::vector<double> prices;
    prices.reserve(trades.size());

    for (std::size_t i = 0; i < trades.size(); ++i) {
        const auto& trade = trades[i];

        prices.push_back(
            price_european_option_mc(
                trade.spot,
                trade.strike,
                trade.maturity,
                trade.risk_free_rate,
                trade.volatility,
                trade.dividend_yield,
                trade.is_call,
                num_paths,
                seed + static_cast<unsigned int>(i)
            )
        );
    }

    return prices;
}

double calculate_portfolio_value_mc(
    const std::vector<black_scholes::OptionTrade>& trades,
    std::size_t num_paths,
    unsigned int seed
) {
    double total_value = 0.0;

    for (std::size_t i = 0; i < trades.size(); ++i) {
        const auto& trade = trades[i];

        const double price = price_european_option_mc(
            trade.spot,
            trade.strike,
            trade.maturity,
            trade.risk_free_rate,
            trade.volatility,
            trade.dividend_yield,
            trade.is_call,
            num_paths,
            seed + static_cast<unsigned int>(i)
        );

        total_value += trade.quantity * price;
    }

    return total_value;
}   

} // namespace risk_engine::monte_carlo