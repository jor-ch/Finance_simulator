#include <iostream>
#include <mutex>
#include "Cash.h"
#include "Stock.h"

void runSimulation(std::mutex &coutMtx)
{
    Stock snp500("../data/SNP500_monthly_price_1871_to_Mar_2026.csv", 1973, 11); // ../ since directory is in build folder, so need to go back one folder
    Cash bankAccount(0.0);                                                       // start with $0
    const double salary = 200.0;
    double totalCashSpent = 0.0;
    for (int i = 0; i < 36; i++)
    {
        bankAccount.updateValue(salary); // simulate person getting salary
        snp500.updateParameters();
        double currentMoneyAvailable = bankAccount.GetValue();
        bankAccount.updateValue(-1.0 * snp500.purchaseStockByPrice(currentMoneyAvailable));
        totalCashSpent += currentMoneyAvailable;
        // std::cout << "bank account has " << bankAccount.GetValue() << " and stock value is "
        //           << snp500.GetValue() << " and stock quantity is "
        //           << snp500.GetStockQuantity() << " and price per stock is "
        //           << snp500.GetPricePerStock() << std::endl;
    }
    {
        std::lock_guard<std::mutex> lock(coutMtx);
        std::cout << "total cash spent is " << totalCashSpent << " and net worth is " << snp500.GetValue() << std::endl;
    }
}
    return 0;
}