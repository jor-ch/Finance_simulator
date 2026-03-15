#pragma once
#include <iostream>

class Asset
{
public:
    double value;
    // this is for updating the asset value after every time cycle, which may differ for every asset type
    //(eg, cash is just adding income - expense, stock is add purchase + scale according to stock price change)
    virtual void updateValue(double change) = 0;

    virtual ~Asset() = default;
};