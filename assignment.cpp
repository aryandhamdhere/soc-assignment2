// Strategy Implementation: RSI + MACD + SMA 
#include "strategy.h"
#include <vector>
#include <cmath>

using namespace std;

//RSI Calculation
// This function calculates RSI (Relative Strength Index)
// RSI tells us if the stock is overbought (>70) or oversold (<30)
double calculate_rsi(const vector<double>& closes, int index, int period = 14) {
    // If we don't have enough data, just return neutral RSI value
    if (index < period) return 50.0;

    double gain = 0, loss = 0;

    // Loop through the last 'period' days to calculate total gain and loss
    for (int i = index - period + 1; i <= index; ++i) {
        double change = closes[i] - closes[i - 1];
        if (change > 0) gain += change;      // Price went up → it's a gain
        else loss -= change;                 // Price went down → count it as a loss (make it positive)
    }

    // If there's no loss at all, RSI is maximum (100)
    if (loss == 0) return 100.0;

    double rs = gain / loss;  // Relative strength = gain/loss
    return 100 - (100 / (1 + rs)); // Final RSI formula (standard)
}

//Exponential Moving Average (EMA) used for MACD 
// This function calculates EMA over a given length
// EMA gives more weight to recent prices
double ema(const vector<double>& data, int current_index, int length) {
    double k = 2.0 / (length + 1); // Smoothing factor
    double ema = data[current_index - length + 1]; // Start EMA with the oldest value in range

    // Calculate EMA using the smoothing formula
    for (int i = current_index - length + 2; i <= current_index; ++i)
        ema = data[i] * k + ema * (1 - k);

    return ema;
}

//MACD Calculation
// MACD = EMA(12) - EMA(26)
// Shows trend direction and momentum
double calculate_macd(const vector<double>& closes, int index) {
    double ema12 = ema(closes, index, 12);
    double ema26 = ema(closes, index, 26);
    return ema12 - ema26;
}

//Simple Moving Average (SMA)
// Calculates simple average of last 'period' closing prices
double calculate_sma(const vector<double>& closes, int index, int period) {
    if (index < period) return closes[index]; // Not enough data, return current price

    double sum = 0;
    for (int i = index - period + 1; i <= index; ++i)
        sum += closes[i];
    return sum / period;
}

// Main Trading Strategy Execution 
// Combines RSI, MACD, and SMA to decide when to buy/sell
TradeResult run_strategy(const vector<Candle>& candles, double profit_threshold) {
    vector<double> closes;
    for (const auto& c : candles) closes.push_back(c.close); // Collect closing prices

    int wins = 0, trades = 0;           // Track successful trades and total trades
    double total_return = 0.0;         // Track total profit/loss
    bool in_position = false;          // Are we currently holding a position?
    double entry_price = 0.0;          // Price at which we entered the trade

    // Start after index 26 so that we have enough data for indicators
    for (int i = 26; i < closes.size(); ++i) {
        double rsi = calculate_rsi(closes, i);         // Get RSI value
        double macd = calculate_macd(closes, i);       // Get MACD value
        double sma20 = calculate_sma(closes, i, 20);   // Get 20-day SMA

        // Buy Logic
        // Conditions to enter a trade:
        // 1. RSI is below 30 → possibly oversold
        // 2. MACD is positive → momentum might be turning bullish
        // 3. Current price is above SMA → price is showing strength
        if (!in_position && rsi < 30 && macd > 0 && closes[i] > sma20) {
            in_position = true;
            entry_price = closes[i];
        }

        // Sell Logic 
        // Conditions to exit a trade:
        // 1. RSI has crossed above 60 (some profit taken)
        // 2. Price falls below SMA (trend weakening)
        else if (in_position && (rsi > 60 || closes[i] < sma20)) {
            double ret = (closes[i] - entry_price) / entry_price;
            total_return += ret;
            if (ret > profit_threshold) wins++;  // Count it as a win if profit > threshold
            trades++;
            in_position = false;
        }
    }

    //If trade is still open at the end, force close it 
    if (in_position) {
        double ret = (closes.back() - entry_price) / entry_price;
        total_return += ret;
        if (ret > profit_threshold) wins++;
        trades++;
    }

    //Final calculation of strategy performance 
    double avg_return = trades ? (total_return / trades) * 100 : 0; // Average return per trade
    double success = trades ? (double)wins / trades * 100 : 0;      // % of profitable trades

    return {success, avg_return, trades, {}}; // Return results
}
