# CrossAssetRiskEngine
A high-performance C++ quantitative finance library for derivative pricing, portfolio risk analysis, Monte Carlo simulation, and fixed-income analytics with seamless Python integration.
---
## Project structure
<img width="415" height="647" alt="Project structure" src="https://github.com/user-attachments/assets/711e8a72-af0a-4e51-bf0f-21d1b10a9b2f" />
---
## Overview
This is a quantative finance library that is designed to provide fast, accurate and extensible pricing and risk analytics for financial instruments. It combines analytical pricing models, the Monte Carlo simulation and fixed-income valuations and risk sensitivities. It is implemented in C++ and exposed through python bindings using pybind11.
---
## Prerequisites
- Python 3.12+
- CMake
- C++20 compatible compiler (Visual Studio Build Tools, GCC, or Clang)
- Git
- 
## Getting Started
Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/CrossAssetRiskEngine.git
cd CrossAssetRiskEngine
```
Build and install the project:

```bash
pip install -e ".[test]"
```
Verify the installation:

```bash
pytest
```
---
## Features

- Analytical Black-Scholes pricing for European call and put options
- Calculation of option Greeks (Delta, Gamma, Vega and Theta)
- Portfolio-level valuation and aggregate risk analysis
- Monte Carlo option pricing using Geometric Brownian Motion
- Fixed-income analytics including Present Value and DV01 calculations
- Python bindings via pybind11 for seamless integration with Python workflows
- Comprehensive unit testing using pytest
- Performance benchmarking comparing analytical pricing, C++ Monte Carlo, and Python Monte Carlo implementations
---
## Software Architecture
<img width="262" height="447" alt="image" src="https://github.com/user-attachments/assets/8f5c6534-ab7d-48c6-a5f3-ad70a41210da" />

This project separates the numerical models from the python interface, the computationally intensive algorithms are implemented in C++ and pybind11 exposes the engine as a native python module. This approach combines C++ performance and the usability of python for testing and benchmarking.
---
## Mathematical Models

├── Black-Scholes 
├── Greeks
├── Monte Carlo
└── Fixed Income

### Black-Scholes
Calculates the theoretical fair market price for European stock options, it provides a closed form benchmark to validate our more complex simulation models

### The Greeks
Measure a portfolio's sensitivity to various market risk factors and it acts as our risk management dashboard

### Monte Carlo
Simulates thousands of asset price paths to value complex financial instruments, uses Brownian motion to simulate millions of potential market scenarios and computes the payoffs for each. It then averages them out to find the asset's present value
 
### Fixed income models 
Model the behaviour of interest rates and value debt instruments like bonds. This ensures the project covers a comprehensive asset spectrum.
---
## Benchmarking and Performance
To evaluate that the performance and the correctness of the implemented models, benchmark tests were performed to compare the Black-Scholes solution, and the C++ and Python Monte-Carlo implementation.

### Accuracy:
Black-Scholes is an analytical solution and gives us an accurate solution, we can use Monte-Carlo approximations to demonstrate how as the number of paths increase, the approximation converges to the analytical price as shown in the graph below.
<img width="910" height="555" alt="Monte Carlo Convergence" src="https://github.com/user-attachments/assets/1d5a0f7b-5199-403a-8efd-89637480469e" />

### Runtime:
More simulation paths, as required by the Monte-Carlo model, increases computiational work and runtime grows approximately linearly with the number of simulations:
<img width="902" height="562" alt="Monte Calro Runtime Scaling" src="https://github.com/user-attachments/assets/328b7323-0131-47af-9650-fbdd1f79797b" />

### C++ vs Python:
Both implementations use the same algorithm and C++ is significantly faster because it is compiled and has lower execution overhead:
<img width="907" height="556" alt="Monte Calro C++ speedup over Python" src="https://github.com/user-attachments/assets/cdd0b954-983f-4e39-afbb-1f49c299d6f3" />
---
## Testing

To ensure the correctness and reliability of the pricing engine, the project includes a comprehensive suite of unit tests implemented using pytest.

The test suite validates both individual pricing models and portfolio-level calculations, covering:

- Analytical Black-Scholes pricing for European call and put options
- Option Greeks (Delta, Gamma, Vega, and Theta)
- Portfolio valuation and aggregate Greek calculations
- Monte Carlo pricing consistency
- Fixed-income Present Value and DV01 calculations
- Input validation and exception handling for invalid parameters

Run the complete test suite using:

```bash
pytest
```
---
## Conclusion

This project provided practical experience in applying quantitative finance theory to real software engineering problems. It combined mathematical modelling, modern C++ development, Python automated testing and performance benchmarking into a single application, reflecting many of the technologies and workflows used in quantitative finance.
