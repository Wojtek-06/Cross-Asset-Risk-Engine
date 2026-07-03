import math
import time
import random
import csv
from pathlib import Path

import _core_risk_engine as risk


def python_monte_carlo_price(
    spot,
    strike,
    maturity,
    risk_free_rate,
    volatility,
    dividend_yield,
    is_call,
    num_paths,
    seed,
):
    random.seed(seed)

    drift = (
        risk_free_rate
        - dividend_yield
        - 0.5 * volatility * volatility
    ) * maturity

    diffusion_scale = volatility * math.sqrt(maturity)

    payoff_sum = 0.0

    for _ in range(num_paths):
        z = random.gauss(0.0, 1.0)
        terminal_spot = spot * math.exp(drift + diffusion_scale * z)

        if is_call:
            payoff = max(terminal_spot - strike, 0.0)
        else:
            payoff = max(strike - terminal_spot, 0.0)

        payoff_sum += payoff

    average_payoff = payoff_sum / num_paths
    return math.exp(-risk_free_rate * maturity) * average_payoff


def time_function(func, *args):
    start = time.perf_counter()
    result = func(*args)
    elapsed = time.perf_counter() - start
    return result, elapsed


def main():
    spot = 100.0
    strike = 100.0
    maturity = 1.0
    rate = 0.05
    volatility = 0.20
    dividend = 0.01
    is_call = True
    seed = 42

    path_counts = [1_000, 10_000, 100_000, 500_000]

    output_dir = Path("benchmark_results")
    output_dir.mkdir(exist_ok=True)

    output_file = output_dir / "pricing_benchmark.csv"

    bsm_price, bsm_time = time_function(
        risk.calculate_price,
        spot,
        strike,
        maturity,
        rate,
        volatility,
        dividend,
        is_call,
    )

    rows = []

    for paths in path_counts:
        cpp_mc_price, cpp_mc_time = time_function(
            risk.price_european_option_mc,
            spot,
            strike,
            maturity,
            rate,
            volatility,
            dividend,
            is_call,
            paths,
            seed,
        )

        py_mc_price, py_mc_time = time_function(
            python_monte_carlo_price,
            spot,
            strike,
            maturity,
            rate,
            volatility,
            dividend,
            is_call,
            paths,
            seed,
        )

        rows.append({
            "paths": paths,
            "black_scholes_price": bsm_price,
            "black_scholes_time": bsm_time,
            "cpp_mc_price": cpp_mc_price,
            "cpp_mc_time": cpp_mc_time,
            "cpp_mc_abs_error": abs(cpp_mc_price - bsm_price),
            "python_mc_price": py_mc_price,
            "python_mc_time": py_mc_time,
            "python_mc_abs_error": abs(py_mc_price - bsm_price),
            "speedup_python_vs_cpp": py_mc_time / cpp_mc_time,
        })

    with output_file.open("w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)

    print(f"Saved benchmark results to {output_file}")

    for row in rows:
        print(row)


if __name__ == "__main__":
    main()