cmake_minimum_required(VERSION 3.20)

project(CrossAssetRiskEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(
    _core_risk_engine
    core/src/bindings.cpp
    core/src/black_scholes.cpp
    core/src/fixed_income.cpp
    core/src/monte_carlo.cpp

)

target_include_directories(
    _core_risk_engine
    PRIVATE
    core/include
)

install(
    TARGETS _core_risk_engine
    LIBRARY DESTINATION .
    RUNTIME DESTINATION .
    ARCHIVE DESTINATION .
)