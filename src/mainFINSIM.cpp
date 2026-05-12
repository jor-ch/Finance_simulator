#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include "Cash.h"
#include "Stock.h"

void runSimulation(std::mutex &coutMtx, int startYear, int startMonth, int durationMonths, int startFunds)
{
    Stock snp500("../data/SNP500_monthly_price_1871_to_Mar_2026.csv", startYear, startMonth); // ../ since directory is in build folder, so need to go back one folder
    Cash bankAccount(0.0);                                                                    // start with $0
    const double salary = 200.0;
    double totalCashSpent = 0.0;
    for (int i = 0; i < 36; i++)
    {
        bankAccount.updateValue(salary); // simulate person getting salary
        snp500.updateParameters();
        double currentMoneyAvailable = bankAccount.GetValue();
        bankAccount.updateValue(-1.0 * snp500.purchaseStockByPrice(currentMoneyAvailable));
        totalCashSpent += currentMoneyAvailable;
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
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++)
    {
        runSimulation(coutMtx_, 1973, 11, 36, 0.0);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "simulation finished in " << duration.count() << std::endl;

    std::cin.get();
    start = std::chrono::high_resolution_clock::now();

    std::thread t1(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t2(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t3(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t4(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t5(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t6(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t7(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t8(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t9(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    std::thread t10(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

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
    std::cout << "simulation finished in " << duration.count() << std::endl;

    return 0;
}