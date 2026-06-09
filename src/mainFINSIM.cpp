#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>
#include <future>
#include <format>
#include <string>
#include <algorithm>
#include "Cash.h"
#include "Stock.h"

// for linux
// #include <sched.h>

std::tuple<double, double> runSimulation(const int startYear,
                                         const int startMonth,
                                         const int durationMonths,
                                         const double startFunds)
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

    return {totalCashSpent, netWorth};
}

// basically a wrapper function so that we can call this function in jthread
// and store the values in the vectors that we pass in by reference
// mutex is for synchronisation of the vectors when multiple threads are trying to write
// to the vectors at the same time
void threadRunSim(const int startYear,
                  const int startMonth,
                  const int durationMonths,
                  const double startFunds,
                  std::vector<int> &startYearVec,
                  std::vector<int> &startMonthVec,
                  std::vector<int> &durationMonthsVec,
                  std::vector<double> &initialInvestmentVec,
                  std::vector<double> &totalCashSpentVec,
                  std::vector<double> &netWorthVec,
                  std::mutex &simulationMutex)
{
    auto [totalCashSpent, netWorth] = runSimulation(startYear,
                                                    startMonth,
                                                    durationMonths,
                                                    startFunds);
    std::lock_guard<std::mutex> lock(simulationMutex);
    startYearVec.push_back(startYear);
    startMonthVec.push_back(startMonth);
    durationMonthsVec.push_back(durationMonths);
    initialInvestmentVec.push_back(startFunds);
    totalCashSpentVec.push_back(totalCashSpent);
    netWorthVec.push_back(netWorth);
}

// for now we set it as one time step = 1 month
int main()
{
    // for inputs
    std::vector<int> startYearVec{1973, 1974};
    std::vector<int> startMonthVec{11, 11};
    std::vector<int> durationMonthsVec{36, 36};
    std::vector<double> initialInvestmentVec{0.0, 0.0};

    // for outputs
    std::vector<double> totalCashSpentVec{};
    std::vector<double> netWorthVec{};
    std::vector<double> percentageGainVec{};

    for (int i = 0; i < startYearVec.size(); ++i)
    {
        auto [totalCashSpent, netWorth] = runSimulation(startYearVec[i],
                                                        startMonthVec[i],
                                                        durationMonthsVec[i],
                                                        initialInvestmentVec[i]);
        totalCashSpentVec.push_back(totalCashSpent);
        netWorthVec.push_back(netWorth);
    }

    std::transform(totalCashSpentVec.begin(),
                   totalCashSpentVec.end(),
                   netWorthVec.begin(),
                   std::back_inserter(percentageGainVec),
                   [](double x, double y)
                   {
                       return (y - x) / x * 100.0;
                   });

    // formatting of header printout
    std::cout << std::format(
        "{:<15}|{:<18}|{:<22}|{:<18}|{:<15}|{:<15}\n",
        "Start(MM/YYYY)",
        "Duration(months)",
        "Initial Investment($)",
        "Cash Spent($)",
        "Net Worth($)",
        "% Gain");

    for (int i = 0; i < totalCashSpentVec.size(); ++i)
    {
        std::cout << std::format(
            "{:<15}|{:<18}|{:<22}|{:<18}|{:<15.2f}|{:<15.5f}\n",
            std::format("{}/{}", startMonthVec[i], startYearVec[i]),
            durationMonthsVec[i],
            initialInvestmentVec[i],
            totalCashSpentVec[i],
            netWorthVec[i],
            percentageGainVec[i]);
    }

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