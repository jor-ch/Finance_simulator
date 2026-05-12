#include <iostream>
#include "Cash.h"
#include "Stock.h"

// for now we set it as one time step = 1 month
int main()
{
    Stock snp500("../data/SNP500_monthly_price_1871_to_Mar_2026.csv", 1973, 11); // ../ since directory is in build folder, so need to go back one folder
    Cash bankAccount(0.0);                                                       // start with $0
    const double salary = 200.0;
    for (int i = 0; i < 10; i++)
    {
        bankAccount.updateValue(salary); // simulate person getting salary
        snp500.updateParameters();
        double currentMoneyAvailable = bankAccount.GetValue();
        bankAccount.updateValue(-1.0 * snp500.purchaseStockByPrice(currentMoneyAvailable));
        std::cout << "bank account has " << bankAccount.GetValue() << " and stock value is "
                  << snp500.GetValue() << " and stock quantity is "
                  << snp500.GetStockQuantity() << " and price per stock is "
                  << snp500.GetPricePerStock() << std::endl;
    }
    return 0;
}