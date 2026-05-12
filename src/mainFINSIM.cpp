#include <iostream>
#include <mutex>
#include <thread>
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

// for now we set it as one time step = 1 month
int main()
{
    std::mutex coutMtx_;

    std::thread t1(runSimulation, std::ref(coutMtx_));
    std::thread t2(runSimulation, std::ref(coutMtx_));
    std::thread t3(runSimulation, std::ref(coutMtx_));
    std::thread t4(runSimulation, std::ref(coutMtx_));
    std::thread t5(runSimulation, std::ref(coutMtx_));
    std::thread t6(runSimulation, std::ref(coutMtx_));
    std::thread t7(runSimulation, std::ref(coutMtx_));
    std::thread t8(runSimulation, std::ref(coutMtx_));
    std::thread t9(runSimulation, std::ref(coutMtx_));
    std::thread t10(runSimulation, std::ref(coutMtx_));


    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();

    return 0;
}