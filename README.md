# C++ Trading Strategy: RSI, MACD, SMA

## What I Learned

While building this project, I explored how to use technical indicators in C++ to make simple trading decisions. Specifically, I learned how to:

* Calculate RSI to spot overbought and oversold zones
* Use MACD to track momentum and trend direction
* Apply SMA to smooth out price data and confirm trends

It was interesting to actually write logic that decides when to buy or sell based on these signals — not just theory anymore.

## Strategy in Simple Terms

* Buy when:

  * RSI < 30 (oversold)
  * MACD > 0 (momentum turning bullish)
  * Price is above 20-day SMA (shows strength)

* Sell when:

  * RSI > 60 (take profits)
  * OR price drops below SMA (trend may weaken)

## Files in This Project

* `strategy.cpp` – Trading logic and indicator calculations
* `strategy.h` – Structs and function declarations
* `README.md` – This summary

Written while learning how trading logic works in practice using C++.
