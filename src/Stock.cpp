#include "Asset.h"
#include "Stock.h"

Stock::Stock(double initial_stock_value)
{
    value = 0.0;
    price_multiplier = 1.0;
    price_per_stock = initial_stock_value;
    quantity = 0.0;
}

void Stock::updateValue(double change)
{
    value = value * price_multiplier + change;
    price_per_stock *= price_multiplier;
    quantity = value / price_per_stock;
}

double Stock::purchaseStock(double stock_quantity) // remember that stock value has to be updated first before purchasing stock,
                                                   // to get the updated stock value before purchase
{
    quantity += stock_quantity;
    double cost_price = stock_quantity * price_per_stock;
    value += cost_price; // assuming no transaction fees
    return cost_price;   // count towards expenses
}
double Stock::sellStock(double stock_quantity) // remember that stock value has to be updated first
{
    quantity -= stock_quantity;
    double cash_gained = stock_quantity * price_per_stock;
    value -= cash_gained;
    return cash_gained; // count towards cash inflow
}