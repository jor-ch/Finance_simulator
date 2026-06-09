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

// question for self: array of structs or struct of arrays?
struct SimulationInputsAndResults
{
    int startYear, startMonth, durationMonths;
    double initialInvestment, totalCashSpent;

    double netWorth, percentageGain;
};

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
    // for inputs (sequential)
    std::vector<int> startYearVec_Seq{};
    std::vector<int> startMonthVec_Seq{};
    std::vector<int> durationMonthsVec_Seq{};
    std::vector<double> initialInvestmentVec_Seq{};

    // for outputs (sequential)
    std::vector<double> totalCashSpentVec_Seq{};
    std::vector<double> netWorthVec_Seq{};
    std::vector<double> percentageGainVec_Seq{};

    // for inputs (concurrent)
    std::vector<int> startYearVec_Con{};
    std::vector<int> startMonthVec_Con{};
    std::vector<int> durationMonthsVec_Con{};
    std::vector<double> initialInvestmentVec_Con{};

    // for outputs (concurrent)
    std::vector<double> totalCashSpentVec_Con{};
    std::vector<double> netWorthVec_Con{};
    std::vector<double> percentageGainVec_Con{};

    // mutex for synchronisation of vectors when multiple threads are trying to
    // write to the vectors at the same time
    std::mutex simulationMutex;

    // sequential approach
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i)
    {
        threadRunSim(1900 + i, 1, 36, 0.0,
                     startYearVec_Seq,
                     startMonthVec_Seq,
                     durationMonthsVec_Seq,
                     initialInvestmentVec_Seq,
                     totalCashSpentVec_Seq,
                     netWorthVec_Seq,
                     simulationMutex);
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::string printout = std::format("sequential simulation finished in {:>10} seconds", duration.count());
    std::cout << printout << std::endl;

    // concurrent approach
    start = std::chrono::high_resolution_clock::now();
    std::vector<std::jthread> threadPool;
    for (int i = 0; i < 100; i++)
    {
        threadPool.emplace_back(threadRunSim, 1900 + i, 1, 36, 0.0,
                                std::ref(startYearVec_Con),
                                std::ref(startMonthVec_Con),
                                std::ref(durationMonthsVec_Con),
                                std::ref(initialInvestmentVec_Con),
                                std::ref(totalCashSpentVec_Con),
                                std::ref(netWorthVec_Con),
                                std::ref(simulationMutex));
    }
    for (auto &thread : threadPool)
    {
        thread.join();
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    printout = std::format("concurrent simulation finished in {:>10} seconds", duration.count());
    std::cout << printout << std::endl;

    // for sequential
    //  calculate percentage gain for each simulation, and store in percentageGainVec
    std::transform(totalCashSpentVec_Seq.begin(),
                   totalCashSpentVec_Seq.end(),
                   netWorthVec_Seq.begin(),
                   std::back_inserter(percentageGainVec_Seq),
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

    for (int i = 0; i < totalCashSpentVec_Seq.size(); ++i)
    {
        std::cout << std::format(
            "{:<15}|{:<18}|{:<22}|{:<18}|{:<15.2f}|{:<15.5f}\n",
            std::format("{}/{}", startMonthVec_Seq[i], startYearVec_Seq[i]),
            durationMonthsVec_Seq[i],
            initialInvestmentVec_Seq[i],
            totalCashSpentVec_Seq[i],
            netWorthVec_Seq[i],
            percentageGainVec_Seq[i]);
    }
    std::cout << std::endl;

    // for concurrent
    //  calculate percentage gain for each simulation, and store in percentageGainVec
    std::transform(totalCashSpentVec_Con.begin(),
                   totalCashSpentVec_Con.end(),
                   netWorthVec_Con.begin(),
                   std::back_inserter(percentageGainVec_Con),
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

    for (int i = 0; i < totalCashSpentVec_Con.size(); ++i)
    {
        std::cout << std::format(
            "{:<15}|{:<18}|{:<22}|{:<18}|{:<15.2f}|{:<15.5f}\n",
            std::format("{}/{}", startMonthVec_Con[i], startYearVec_Con[i]),
            durationMonthsVec_Con[i],
            initialInvestmentVec_Con[i],
            totalCashSpentVec_Con[i],
            netWorthVec_Con[i],
            percentageGainVec_Con[i]);
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