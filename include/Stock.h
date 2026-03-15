#pragma once
#include "Asset.h"
class Stock : public Asset
{
public:
    Stock(double initial_stock_value);
    double price_multiplier; // multiplier to see stock scaling from previous t to current t
    double price_per_stock;
    double quantity; // number of the stock the person has
    void updateValue(double change) override;
    double purchaseStock(double stock_quantity);
    double sellStock(double stock_quantity);
};