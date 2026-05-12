#include <iostream>
#include "Cash.h"
#include "Stock.h"

// for now we set it as one time step = 1 month
int main()
{
    Stock snp500("../data/SNP500_monthly_price_1871_to_Mar_2026.csv", 1973, 11); // ../ since directory is in build folder, so need to go back one folder
    double stock_expense = snp500.purchaseStock(1.0);
    std::cout << "stock expense is " << stock_expense << " and value is " << snp500.value << std::endl;
    for (int i = 0; i < 10; i++)
    {
        snp500.updateParameters();
        std::cout << "price per stock now is " << snp500.GetPricePerStock() << std::endl;
        std::cout << "current stock value before purchase is " << snp500.GetValue() << std::endl;
        if (i % 2 == 0)
        {
            stock_expense = snp500.purchaseStock(1.0);
        }
        else
        {
            stock_expense = snp500.purchaseStockByPrice(200);
        }

        std::cout << "stock quantity is " << snp500.GetStockQuantity() << " and stock value is " << snp500.GetValue() << std::endl;
    }
    return 0;
}