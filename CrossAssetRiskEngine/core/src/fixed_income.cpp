#include "fixed_income.hpp"

#include <cmath>
#include <stdexcept>

namespace risk_engine::fixed_income {

namespace {

void validate_cashflows(
    const std::vector<double>& amounts,
    const std::vector<double>& times
) {
    if (amounts.size() != times.size()) {
        throw std::invalid_argument(
            "Cash-flow amounts and times must have the same length."
        );
    }

    if (amounts.empty()) {
        throw std::invalid_argument(
            "Cash-flow amounts and times cannot be empty."
        );
    }
}

} // namespace

double calculate_pv(
    const std::vector<double>& amounts,
    const std::vector<double>& times,
    double yield
) {
    validate_cashflows(amounts, times);

    double present_value = 0.0;

    for (std::size_t i = 0; i < amounts.size(); ++i) {
        present_value += amounts[i] * std::exp(-yield * times[i]);
    }

    return present_value;
}

double calculate_dv01(
    const std::vector<double>& amounts,
    const std::vector<double>& times,
    double yield
) {
    validate_cashflows(amounts, times);

    constexpr double half_basis_point = 0.00005;

    const double pv_down = calculate_pv(
        amounts,
        times,
        yield - half_basis_point
    );

    const double pv_up = calculate_pv(
        amounts,
        times,
        yield + half_basis_point
    );

    return (pv_down - pv_up) / 2.0;
}

} // namespace risk_engine::fixed_income