#include <iostream>
#include "Cash.h"
#include "Stock.h"

int main()
{
    Cash client_cash(2000.0);
    Stock snp500(350.0);
    double stock_expense = snp500.purchaseStock(3.0);
    client_cash.updateValue(-stock_expense);
    std::cout << "client stock quantity and value is " << snp500.quantity
              << " , " << snp500.value << std::endl;
    std::cout << "client is left with " << client_cash.value << std::endl;
    return 0;
}