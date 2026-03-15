#pragma once
#include "Asset.h"
class Cash : public Asset
{
public:
    Cash(double initial_value);
    void setInitialValue(double initial_value);
    void updateValue(double change) override;
};