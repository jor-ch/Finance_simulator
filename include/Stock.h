#pragma once
#include "Asset.h"
#include <vector>
class Stock : public Asset
{
public:
    Stock(double initial_stock_value);
    Stock(std::string past_stock_data_file);          // this constructor is for initialising the stock price multiplier based on past data, which is used to predict future stock price movement
    std::vector<double> past_stock_prices;            // this is for storing past stock price data, which is used to predict future stock price movement
    std::vector<double> past_stock_price_multipliers; // this is for storing past stock price multipliers, which is used to predict future stock price movement
    double price_multiplier_from_t_0;                 // multiplier to see how much the stock has scaled since t = 0, for logging purposes
    double price_multiplier;                          // multiplier to see stock scaling from previous t to current t
    double price_per_stock;
    double quantity; // number of the stock the person has
    void updateValue(double change) override;
    void updateStockMultiplier(); // plan is to initialise a model to predict how the price would move, could be a random number for simplicity sake
    double purchaseStock(double stock_quantity);
    double sellStock(double stock_quantity);

    int firstYear, firstMonth; // this is for storing the date of the first stock price data,
                               // which is used to calculate the date of the current stock price data
                               // based on the number of time cycles that have passed
};