#include <iostream>
#include "Cash.h"
#include "Stock.h"

int main()
{
    Stock snp500("../data/SNP500_monthly_price_1871_to_Mar_2026.csv"); // ../ since directory is in build folder, so need to go back one folder
    // Cash client_cash(2000.0);
    // Stock snp500(350.0);
    // double stock_expense = snp500.purchaseStock(3.0);
    // snp500.updateStockMultiplier();
    // snp500.updateValue(0.0);
    // client_cash.updateValue(-stock_expense);
    // std::cout << "client stock quantity and value is " << snp500.quantity
    //           << " , " << snp500.value << std::endl;
    // std::cout << "client is left with " << client_cash.value << std::endl;
    return 0;
}