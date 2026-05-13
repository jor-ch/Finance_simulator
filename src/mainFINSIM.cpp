#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <future>
#include <ranges>
#include <format>
#include <string>
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
        double moneySpent = snp500.purchaseStockByPrice(currentMoneyAvailable);
        bankAccount.updateValue(-1.0 * moneySpent);
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

    // for inputs
    std::vector<int> startYearVec{1973, 1974};
    std::vector<int> startMonthVec{11, 11};
    std::vector<int> durationMonthsVec{36, 36};
    std::vector<double> initialInvestmentVec{0.0, 0.0};

    // for outputs
    std::vector<double> totalCashSpentVec{};
    std::vector<double> netWorthVec{};
    std::vector<double> percentageGainVec{};
    std::vector<double> pcntGaintest(totalCashSpentVec.size());

    for (int i = 0; i < startYearVec.size(); ++i)
    {
        auto [totalCashSpent, netWorth, percentageGain] = runSimulation(coutMtx_,
                                                                        startYearVec[i],
                                                                        startMonthVec[i],
                                                                        durationMonthsVec[i],
                                                                        initialInvestmentVec[i]);
        totalCashSpentVec.push_back(totalCashSpent);
        netWorthVec.push_back(netWorth);
        percentageGainVec.push_back(percentageGain);
    }

    // formatting of header printout
    std::cout << std::format(
        "{:<20} | {:<20} | {:<20} | {:<20} | {:<20}\n",
        "Start Date(MM/YYYY)",
        "Duration(months)",
        "Total Cash Spent",
        "Net Worth",
        "% Gain");
    std::cout << std::format(
        "{:<20} | {:<20} | {:<20} | {:<20} | {:<20}\n",
        std::format("{}/{}", startMonth, startYear),
        durationMonths,
        totalCashSpent,
        netWorth,
        percentageGain);

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