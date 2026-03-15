#pragma once
#include "Asset.h"
class Stock : public Asset
{
public:
    Stock(double initial_stock_value);
    double price_multiplier_from_t_0; // multiplier to see how much the stock has scaled since t = 0, for logging purposes
    double price_multiplier;          // multiplier to see stock scaling from previous t to current t
    double price_per_stock;
    double quantity; // number of the stock the person has
    void updateValue(double change) override;
    void updateStockMultiplier(); // plan is to initialise a model to predict how the price would move, could be a random number for simplicity sake
    double purchaseStock(double stock_quantity);
    double sellStock(double stock_quantity);
};