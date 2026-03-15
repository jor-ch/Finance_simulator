#include "Asset.h"
#include "Cash.h"

Cash::Cash(double initial_value)
{
    setInitialValue(initial_value);
}

void Cash::setInitialValue(double initial_value)
{
    value = initial_value;
}

void Cash::updateValue(double change)
{
    value += change; // since for cash, value is just addition of the change in cash after every time period
}