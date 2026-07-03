#include "black_scholes.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace risk_engine::black_scholes {

namespace {


void validate_inputs(
    double spot,
    double strike,
    double maturity,
    double volatility
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
}

double normal_cdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

double normal_pdf(double x) {
    return (1.0 / std::sqrt(2.0 * std::numbers::pi))
        * std::exp(-0.5 * x * x);
}

double calculate_d1(
    double spot,
    double strike,
    double maturity,
    double risk_free_rate,
    double volatility,
    double dividend_yield
) {
    return (
        std::log(spot / strike)
        + (risk_free_rate - dividend_yield + 0.5 * volatility * volatility) * maturity
    ) / (volatility * std::sqrt(maturity));
}

double calculate_d2(
    double d1,
    double volatility,
    double maturity
) {
    return d1 - volatility * std::sqrt(maturity);
}

} //namespace

double calculate_price(
    double spot,
    double strike,
    double maturity,
    double risk_free_rate,
    double volatility,
    double dividend_yield,
    bool is_call
    
) {
    validate_inputs(spot, strike, maturity, volatility);

    const double d1 = calculate_d1(
        spot,
        strike,
        maturity,
        risk_free_rate,
        volatility,
        dividend_yield
    );

    const double d2 = calculate_d2(d1, volatility, maturity);

    const double discounted_spot =
        spot * std::exp(-dividend_yield * maturity);

    const double discounted_strike =
        strike * std::exp(-risk_free_rate * maturity);

    if (is_call) {
        return discounted_spot * normal_cdf(d1)
             - discounted_strike * normal_cdf(d2);
    }

    return discounted_strike * normal_cdf(-d2)
         - discounted_spot * normal_cdf(-d1);
} 


OptionGreeks calculate_greeks(
    double spot,
    double strike,
    double maturity,
    double risk_free_rate,
    double volatility,
    double dividend_yield,
    bool is_call
) {
    validate_inputs(spot, strike, maturity, volatility);

    const double d1 = calculate_d1(
        spot,
        strike,
        maturity,
        risk_free_rate,
        volatility,
        dividend_yield
    );

    const double d2 = calculate_d2(d1, volatility, maturity);

    const double discounted_dividend =
        std::exp(-dividend_yield * maturity);

    const double discounted_strike =
        strike * std::exp(-risk_free_rate * maturity);

    const double pdf_d1 = normal_pdf(d1);
    const double sqrt_maturity = std::sqrt(maturity);

    const double delta = is_call
        ? discounted_dividend * normal_cdf(d1)
        : discounted_dividend * (normal_cdf(d1) - 1.0);

    const double gamma =
        (discounted_dividend * pdf_d1)
        / (spot * volatility * sqrt_maturity);

    const double raw_vega =
        spot * discounted_dividend * sqrt_maturity * pdf_d1;

    const double vega_1bp = raw_vega * 0.0001;

    const double first_theta_term =
        -(spot * discounted_dividend * pdf_d1 * volatility)
        / (2.0 * sqrt_maturity);

    const double theta = is_call
        ? first_theta_term
            - risk_free_rate * discounted_strike * normal_cdf(d2)
            + dividend_yield * spot * discounted_dividend * normal_cdf(d1)
        : first_theta_term
            + risk_free_rate * discounted_strike * normal_cdf(-d2)
            - dividend_yield * spot * discounted_dividend * normal_cdf(-d1);

    return OptionGreeks{
        delta,
        gamma,
        vega_1bp,
        theta
    };
} 

std::vector<double> calculate_prices(
    const std::vector<OptionTrade>& trades
) {
    std::vector<double> prices;
    prices.reserve(trades.size());

    for (const OptionTrade& trade : trades) {
        prices.push_back(
            calculate_price(
                trade.spot,
                trade.strike,
                trade.maturity,
                trade.risk_free_rate,
                trade.volatility,
                trade.dividend_yield,
                trade.is_call
            )
        );
    }

    return prices;
}

std::vector<OptionGreeks> calculate_greeks(
    const std::vector<OptionTrade>& trades
) {
    std::vector<OptionGreeks> greeks;
    greeks.reserve(trades.size());

    for (const OptionTrade& trade : trades) {
        greeks.push_back(
            calculate_greeks(
                trade.spot,
                trade.strike,
                trade.maturity,
                trade.risk_free_rate,
                trade.volatility,
                trade.dividend_yield,
                trade.is_call
            )
        );
    }

    return greeks;
}


PortfolioGreeks aggregate_greeks(
    const std::vector<OptionGreeks>& greeks
) {
    PortfolioGreeks total{
        0.0,
        0.0,
        0.0,
        0.0
    };

    for (const OptionGreeks& g : greeks) {
        total.delta += g.delta;
        total.gamma += g.gamma;
        total.vega_1bp += g.vega_1bp;
        total.theta += g.theta;
    }

    return total;
}



double calculate_portfolio_value(
    const std::vector<OptionTrade>& trades
) {
    double total_value = 0.0;

    for (const OptionTrade& trade : trades) {
        const double price = calculate_price(
            trade.spot,
            trade.strike,
            trade.maturity,
            trade.risk_free_rate,
            trade.volatility,
            trade.dividend_yield,
            trade.is_call
        );

        total_value += trade.quantity * price;
    }

    return total_value;
}

PortfolioGreeks calculate_portfolio_greeks(
    const std::vector<OptionTrade>& trades
) {
    PortfolioGreeks total{
        0.0,
        0.0,
        0.0,
        0.0
    };

    for (const OptionTrade& trade : trades) {
        const OptionGreeks greeks = calculate_greeks(
            trade.spot,
            trade.strike,
            trade.maturity,
            trade.risk_free_rate,
            trade.volatility,
            trade.dividend_yield,
            trade.is_call
        );

        total.delta += trade.quantity * greeks.delta;
        total.gamma += trade.quantity * greeks.gamma;
        total.vega_1bp += trade.quantity * greeks.vega_1bp;
        total.theta += trade.quantity * greeks.theta;
    }

    return total;
}

} //namespace risk_engine::black_scholes