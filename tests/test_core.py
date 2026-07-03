import math

import pytest

import _core_risk_engine as risk


def test_put_call_parity():
    spot = 100.0
    strike = 100.0
    maturity = 1.0
    rate = 0.05
    volatility = 0.20
    dividend = 0.01

    call = risk.calculate_price(
        spot,
        strike,
        maturity,
        rate,
        volatility,
        dividend,
        True,
    )

    put = risk.calculate_price(
        spot,
        strike,
        maturity,
        rate,
        volatility,
        dividend,
        False,
    )

    lhs = call - put
    rhs = (
        spot * math.exp(-dividend * maturity)
        - strike * math.exp(-rate * maturity)
    )

    assert lhs == pytest.approx(rhs, rel=1e-10)


def test_greeks():
    greeks = risk.calculate_greeks(
        100.0,
        100.0,
        1.0,
        0.05,
        0.20,
        0.01,
        True,
    )

    assert 0.0 < greeks["delta"] < 1.0
    assert greeks["gamma"] > 0.0
    assert greeks["vega_1bp"] > 0.0


def test_invalid_inputs():
    with pytest.raises(ValueError):
        risk.calculate_price(
            100.0,
            100.0,
            0.0,
            0.05,
            0.20,
            0.01,
            True,
        )

    with pytest.raises(ValueError):
        risk.calculate_price(
            -100.0,
            100.0,
            1.0,
            0.05,
            0.20,
            0.01,
            True,
        )

    with pytest.raises(ValueError):
        risk.calculate_price(
            100.0,
            -100.0,
            1.0,
            0.05,
            0.20,
            0.01,
            True,
        )


def test_present_value():
    amounts = [5.0, 5.0, 105.0]
    times = [1.0, 2.0, 3.0]

    pv = risk.calculate_pv(amounts, times, 0.04)

    assert pv > 0.0


def test_dv01():
    amounts = [5.0, 5.0, 105.0]
    times = [1.0, 2.0, 3.0]

    dv01 = risk.calculate_dv01(amounts, times, 0.04)

    assert dv01 > 0.0

def test_batch_option_pricing():
    trade_1 = risk.OptionTrade()
    trade_1.spot = 100.0
    trade_1.strike = 100.0
    trade_1.maturity = 1.0
    trade_1.risk_free_rate = 0.05
    trade_1.volatility = 0.20
    trade_1.dividend_yield = 0.01
    trade_1.is_call = True
    trade_1.quantity = 1.0

    trade_2 = risk.OptionTrade()
    trade_2.spot = 100.0
    trade_2.strike = 100.0
    trade_2.maturity = 1.0
    trade_2.risk_free_rate = 0.05
    trade_2.volatility = 0.20
    trade_2.dividend_yield = 0.01
    trade_2.is_call = False
    trade_2.quantity = 1.0

    prices = risk.calculate_prices([trade_1, trade_2])

    assert len(prices) == 2
    assert prices[0] > 0.0
    assert prices[1] > 0.0

def test_portfolio_greeks():
    trade_1 = risk.OptionTrade()
    trade_1.spot = 100.0
    trade_1.strike = 100.0
    trade_1.maturity = 1.0
    trade_1.risk_free_rate = 0.05
    trade_1.volatility = 0.20
    trade_1.dividend_yield = 0.01
    trade_1.is_call = True
    trade_1.quantity = 1.0

    trade_2 = risk.OptionTrade()
    trade_2.spot = 100.0
    trade_2.strike = 100.0
    trade_2.maturity = 1.0
    trade_2.risk_free_rate = 0.05
    trade_2.volatility = 0.20
    trade_2.dividend_yield = 0.01
    trade_2.is_call = False
    trade_2.quantity = 1.0

    portfolio_greeks = risk.calculate_portfolio_greeks([trade_1, trade_2])

    assert "delta" in portfolio_greeks
    assert "gamma" in portfolio_greeks
    assert "vega_1bp" in portfolio_greeks
    assert "theta" in portfolio_greeks

    assert portfolio_greeks["gamma"] > 0.0
    assert portfolio_greeks["vega_1bp"] > 0.0

def test_portfolio_value_uses_quantity():
    trade_1 = risk.OptionTrade()
    trade_1.spot = 100.0
    trade_1.strike = 100.0
    trade_1.maturity = 1.0
    trade_1.risk_free_rate = 0.05
    trade_1.volatility = 0.20
    trade_1.dividend_yield = 0.01
    trade_1.is_call = True
    trade_1.quantity = 100.0

    trade_2 = risk.OptionTrade()
    trade_2.spot = 100.0
    trade_2.strike = 100.0
    trade_2.maturity = 1.0
    trade_2.risk_free_rate = 0.05
    trade_2.volatility = 0.20
    trade_2.dividend_yield = 0.01
    trade_2.is_call = False
    trade_2.quantity = -50.0

    expected = (
        trade_1.quantity * risk.calculate_price(100.0, 100.0, 1.0, 0.05, 0.20, 0.01, True)
        + trade_2.quantity * risk.calculate_price(100.0, 100.0, 1.0, 0.05, 0.20, 0.01, False)
    )

    actual = risk.calculate_portfolio_value([trade_1, trade_2])

    assert actual == pytest.approx(expected)

def test_portfolio_greeks_use_quantity():
    trade = risk.OptionTrade()
    trade.spot = 100.0
    trade.strike = 100.0
    trade.maturity = 1.0
    trade.risk_free_rate = 0.05
    trade.volatility = 0.20
    trade.dividend_yield = 0.01
    trade.is_call = True
    trade.quantity = 100.0

    single = risk.calculate_greeks(
        trade.spot,
        trade.strike,
        trade.maturity,
        trade.risk_free_rate,
        trade.volatility,
        trade.dividend_yield,
        trade.is_call,
    )

    portfolio = risk.calculate_portfolio_greeks([trade])

    assert portfolio["delta"] == pytest.approx(100.0 * single["delta"])
    assert portfolio["gamma"] == pytest.approx(100.0 * single["gamma"])
    assert portfolio["vega_1bp"] == pytest.approx(100.0 * single["vega_1bp"])
    assert portfolio["theta"] == pytest.approx(100.0 * single["theta"])
def test_monte_carlo_price_close_to_black_scholes():
    bsm_price = risk.calculate_price(
        100.0,
        100.0,
        1.0,
        0.05,
        0.20,
        0.01,
        True,
    )

    mc_price = risk.price_european_option_mc(
        100.0,
        100.0,
        1.0,
        0.05,
        0.20,
        0.01,
        True,
        200_000,
        42,
    )

    assert mc_price == pytest.approx(bsm_price, rel=0.03)

def test_monte_carlo_batch_pricing():
    trade = risk.OptionTrade()
    trade.spot = 100.0
    trade.strike = 100.0
    trade.maturity = 1.0
    trade.risk_free_rate = 0.05
    trade.volatility = 0.20
    trade.dividend_yield = 0.01
    trade.is_call = True
    trade.quantity = 10.0

    prices = risk.price_options_mc([trade], 100_000, 42)

    assert len(prices) == 1
    assert prices[0] > 0.0
def test_monte_carlo_portfolio_value():
    trade = risk.OptionTrade()
    trade.spot = 100.0
    trade.strike = 100.0
    trade.maturity = 1.0
    trade.risk_free_rate = 0.05
    trade.volatility = 0.20
    trade.dividend_yield = 0.01
    trade.is_call = True
    trade.quantity = 100.0

    mc_single_price = risk.price_european_option_mc(
        100.0, 100.0, 1.0, 0.05, 0.20, 0.01, True, 100_000, 42
    )

    mc_portfolio_value = risk.calculate_portfolio_value_mc(
        [trade], 100_000, 42
    )

    assert mc_portfolio_value == pytest.approx(
        trade.quantity * mc_single_price
    )