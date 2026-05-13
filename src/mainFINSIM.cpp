#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <future>
#include <ranges>
#include <format>
#include "Cash.h"
#include "Stock.h"

// for linux
// #include <sched.h>

std::tuple<double, double, double> runSimulation(std::mutex &coutMtx,
                                                 int startYear,
                                                 int startMonth,
                                                 int durationMonths,
                                                 int startFunds)
{
    // int cpuinit = sched_getcpu();
    Stock snp500("../data/SNP500_monthly_price_1871_to_Mar_2026.csv", startYear, startMonth); // ../ since directory is in build folder, so need to go back one folder
    Cash bankAccount(startFunds);                                                             // start with $0
    const double salary = 200.0;
    double totalCashSpent = 0.0;
    double netWorth = 0.0;
    for (int i = 0; i < durationMonths; i++)
    {
        bankAccount.updateValue(salary); // simulate person getting salary
        snp500.updateParameters();
        double currentMoneyAvailable = bankAccount.GetValue();
        bankAccount.updateValue(-1.0 * snp500.purchaseStockByPrice(currentMoneyAvailable));
        totalCashSpent += currentMoneyAvailable;
        netWorth = bankAccount.GetValue() + snp500.GetValue();
    }
    // {
    //     std::lock_guard<std::mutex> lock(coutMtx);
    //     std::cout << "total cash spent is " << totalCashSpent << " and net worth is " << netWorth << std::endl;
    // }
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // int cpuend = sched_getcpu();
    // {
    //     std::lock_guard<std::mutex> lock(coutMtx);
    //     std::cout << "simulation started on CPU " << cpuinit << " and ended on CPU " << cpuend << std::endl;
    // }
    double percentageGain = (netWorth - totalCashSpent) / totalCashSpent * 100.0;

    return {totalCashSpent, netWorth, percentageGain};
}

// for now we set it as one time step = 1 month
int main()
{
    std::mutex coutMtx_;

    auto [totalCashSpent, netWorth, percentageGain] = runSimulation(coutMtx_, 1973, 11, 36, 0.0);
    std::cout << "total cash spent is " << totalCashSpent
              << " and net worth is " << netWorth
              << " and percentage gain is " << percentageGain
              << "%" << std::endl;

    // concurrent runing of 50 simulations
    // auto start = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < 50; i++)
    // {
    //     runSimulation(coutMtx_, 1973, 11, 36, 0.0);
    // }

    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // std::string printout = std::format("simulation finished in {:>10} microseconds", duration.count());
    // std::cout << printout << std::endl;

    // // concurrent running of 50 simulations using threads
    // // std::cin.get();
    // start = std::chrono::high_resolution_clock::now();
    // std::vector<std::jthread> threadPool;
    // for (int i = 0; i < 50; i++)
    // {
    //     threadPool.emplace_back(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // }
    // end = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // printout = std::format("simulation finished in {:>10} microseconds", duration.count());
    // std::cout << printout << std::endl;

    // // concurrent running of 50 simulations using async
    // start = std::chrono::high_resolution_clock::now();
    // std::vector<std::future<void>> futurePool;
    // for (int i = 0; i < 50; i++)
    // {
    //     futurePool.emplace_back(std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0));
    // }
    // for (auto &future : futurePool)
    // {
    //     future.get();
    // }
    // end = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // printout = std::format("simulation finished in {:>10} microseconds", duration.count());
    // std::cout << printout << std::endl;

    return 0;
}