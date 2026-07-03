#pragma once

#include <vector>

namespace risk_engine::fixed_income {

/**
 * @brief Calculates the present value of a set of deterministic cash flows.
 *
 * Formula:
 * PV = sum(amount_i * exp(-yield * time_i))
 *
 * @param amounts Cash-flow amounts.
 * @param times Payment times in years.
 * @param yield Continuously compounded yield.
 *
 * @return Present value of the cash-flow stream.
 */
double calculate_pv(
    const std::vector<double>& amounts,
    const std::vector<double>& times,
    double yield
);

/**
 * @brief Calculates DV01 using a central finite-difference approximation.
 *
 * The curve is bumped up and down by 0.5 basis points.
 *
 * DV01 = (PV(yield - 0.00005) - PV(yield + 0.00005)) / 2
 *
 * @param amounts Cash-flow amounts.
 * @param times Payment times in years.
 * @param yield Continuously compounded yield.
 *
 * @return Dollar value of one basis point.
 */
double calculate_dv01(
    const std::vector<double>& amounts,
    const std::vector<double>& times,
    double yield
);

} // namespace risk_engine::fixed_income