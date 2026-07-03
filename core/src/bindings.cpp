#include "black_scholes.hpp"
#include "fixed_income.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "monte_carlo.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(_core_risk_engine, module) {
    module.doc() = "C++ core risk engine for cross-asset portfolio analytics";

    py::class_<risk_engine::black_scholes::OptionTrade>(module, "OptionTrade")
        .def(py::init<>())
        .def_readwrite("spot", &risk_engine::black_scholes::OptionTrade::spot)
        .def_readwrite("strike", &risk_engine::black_scholes::OptionTrade::strike)
        .def_readwrite("maturity", &risk_engine::black_scholes::OptionTrade::maturity)
        .def_readwrite("risk_free_rate", &risk_engine::black_scholes::OptionTrade::risk_free_rate)
        .def_readwrite("volatility", &risk_engine::black_scholes::OptionTrade::volatility)
        .def_readwrite("dividend_yield", &risk_engine::black_scholes::OptionTrade::dividend_yield)
        .def_readwrite("is_call", &risk_engine::black_scholes::OptionTrade::is_call)
        .def_readwrite("quantity", &risk_engine::black_scholes::OptionTrade::quantity);

    module.def(
        "calculate_price",
        &risk_engine::black_scholes::calculate_price,
        py::arg("spot"),
        py::arg("strike"),
        py::arg("maturity"),
        py::arg("risk_free_rate"),
        py::arg("volatility"),
        py::arg("dividend_yield"),
        py::arg("is_call")
    );

    module.def(
        "calculate_prices",
        &risk_engine::black_scholes::calculate_prices,
        py::arg("trades")
    );

    module.def(
        "calculate_greeks",
        [](double spot,
           double strike,
           double maturity,
           double risk_free_rate,
           double volatility,
           double dividend_yield,
           bool is_call) {
            const auto greeks = risk_engine::black_scholes::calculate_greeks(
                spot,
                strike,
                maturity,
                risk_free_rate,
                volatility,
                dividend_yield,
                is_call
            );

            return py::dict(
                "delta"_a = greeks.delta,
                "gamma"_a = greeks.gamma,
                "vega_1bp"_a = greeks.vega_1bp,
                "theta"_a = greeks.theta
            );
        },
        py::arg("spot"),
        py::arg("strike"),
        py::arg("maturity"),
        py::arg("risk_free_rate"),
        py::arg("volatility"),
        py::arg("dividend_yield"),
        py::arg("is_call")
    );

    module.def(
        "calculate_pv",
        &risk_engine::fixed_income::calculate_pv,
        py::arg("amounts"),
        py::arg("times"),
        py::arg("yield")
    );

    module.def(
        "calculate_dv01",
        &risk_engine::fixed_income::calculate_dv01,
        py::arg("amounts"),
        py::arg("times"),
        py::arg("yield")
    );


    module.def(
        "calculate_greeks_batch",
        [](const std::vector<risk_engine::black_scholes::OptionTrade>& trades) {
            const auto batch_greeks =
                risk_engine::black_scholes::calculate_greeks(trades);
    
            py::list results;
    
            for (const auto& greeks : batch_greeks) {
                results.append(
                    py::dict(
                        "delta"_a = greeks.delta,
                        "gamma"_a = greeks.gamma,
                        "vega_1bp"_a = greeks.vega_1bp,
                        "theta"_a = greeks.theta
                    )
                );
            }
    
            return results;
        },
        py::arg("trades")
    );

    module.def(
        "calculate_portfolio_greeks",
        [](const std::vector<risk_engine::black_scholes::OptionTrade>& trades) {
            const auto greeks =
                risk_engine::black_scholes::calculate_portfolio_greeks(trades);
    
            return py::dict(
                "delta"_a = greeks.delta,
                "gamma"_a = greeks.gamma,
                "vega_1bp"_a = greeks.vega_1bp,
                "theta"_a = greeks.theta
            );
        },
        py::arg("trades")
    );

    module.def(
        "calculate_portfolio_value",
        &risk_engine::black_scholes::calculate_portfolio_value,
        py::arg("trades")
    );

    module.def(
        "price_european_option_mc",
        &risk_engine::monte_carlo::price_european_option_mc,
        py::arg("spot"),
        py::arg("strike"),
        py::arg("maturity"),
        py::arg("risk_free_rate"),
        py::arg("volatility"),
        py::arg("dividend_yield"),
        py::arg("is_call"),
        py::arg("num_paths"),
        py::arg("seed")
    );
    
    module.def(
        "price_options_mc",
        &risk_engine::monte_carlo::price_options_mc,
        py::arg("trades"),
        py::arg("num_paths"),
        py::arg("seed")
    );

    module.def(
        "calculate_portfolio_value_mc",
        &risk_engine::monte_carlo::calculate_portfolio_value_mc,
        py::arg("trades"),
        py::arg("num_paths"),
        py::arg("seed")
    );
}