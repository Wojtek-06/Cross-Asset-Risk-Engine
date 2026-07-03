from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt


def save_plot(filename):
    output_path = Path("benchmark_results") / filename
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"Saved {output_path}")


def main():
    results_dir = Path("benchmark_results")
    input_file = results_dir / "pricing_benchmark.csv"

    df = pd.read_csv(input_file)

    plt.rcParams.update({
        "figure.figsize": (9, 5.5),
        "axes.grid": True,
        "grid.alpha": 0.25,
        "axes.spines.top": False,
        "axes.spines.right": False,
        "font.size": 11,
        "axes.titlesize": 15,
        "axes.labelsize": 12,
        "legend.frameon": False,
    })

    # 1. Monte Carlo convergence
    plt.figure()
    plt.plot(
        df["paths"],
        df["cpp_mc_abs_error"],
        marker="o",
        linewidth=2.2,
        label="C++ Monte Carlo",
    )
    plt.plot(
        df["paths"],
        df["python_mc_abs_error"],
        marker="o",
        linewidth=2.2,
        label="Python Monte Carlo",
    )
    plt.xscale("log")
    plt.xlabel("Monte Carlo paths")
    plt.ylabel("Absolute pricing error")
    plt.title("Monte Carlo convergence to Black-Scholes")
    plt.legend()
    plt.figtext(
        0.01,
        0.01,
        "Lower is better. Error measured against analytical Black-Scholes price.",
        fontsize=9,
        alpha=0.75,
    )
    save_plot("mc_error_vs_paths_polished.png")

    # 2. Runtime scaling
    plt.figure()
    plt.plot(
        df["paths"],
        df["cpp_mc_time"],
        marker="o",
        linewidth=2.2,
        label="C++ Monte Carlo",
    )
    plt.plot(
        df["paths"],
        df["python_mc_time"],
        marker="o",
        linewidth=2.2,
        label="Python Monte Carlo",
    )
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Monte Carlo paths")
    plt.ylabel("Runtime, seconds")
    plt.title("Monte Carlo runtime scaling")
    plt.legend()
    plt.figtext(
        0.01,
        0.01,
        "Log-log scale highlights how runtime grows as simulation count increases.",
        fontsize=9,
        alpha=0.75,
    )
    save_plot("runtime_vs_paths_polished.png")

    # 3. C++ speedup over Python
    plt.figure()
    plt.plot(
        df["paths"],
        df["speedup_python_vs_cpp"],
        marker="o",
        linewidth=2.4,
    )
    plt.xscale("log")
    plt.xlabel("Monte Carlo paths")
    plt.ylabel("Speedup multiple")
    plt.title("C++ Monte Carlo speedup over Python")

    max_speedup = df["speedup_python_vs_cpp"].max()
    max_row = df.loc[df["speedup_python_vs_cpp"].idxmax()]
    plt.annotate(
        f"Peak speedup: {max_speedup:.1f}x",
        xy=(max_row["paths"], max_speedup),
        xytext=(max_row["paths"], max_speedup - 2.0),
        arrowprops={"arrowstyle": "->", "lw": 1.2},
        fontsize=10,
    )

    plt.figtext(
        0.01,
        0.01,
        "Speedup = Python runtime divided by C++ runtime.",
        fontsize=9,
        alpha=0.75,
    )
    save_plot("cpp_speedup_vs_paths_polished.png")


if __name__ == "__main__":
    main()