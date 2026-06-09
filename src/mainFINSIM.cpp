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
    double initialInvestment;

    double totalCashSpent, netWorth, percentageGain;
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
                  std::vector<SimulationInputsAndResults> &simParametersVec,
                  std::mutex &simulationMutex)
{
    auto [totalCashSpent, netWorth] = runSimulation(startYear,
                                                    startMonth,
                                                    durationMonths,
                                                    startFunds);
    std::lock_guard<std::mutex> lock(simulationMutex);
    // note: the design is done such that storing is only for simulation parameters and results;
    // other parameterslike percentage gain would be done separately.
    // it may be more efficient to calculate it here but I want to separate the calculations
    // and post-processing with the actual simulation run
    simParametersVec.emplace_back(SimulationInputsAndResults{startYear,
                                                             startMonth,
                                                             durationMonths,
                                                             startFunds,
                                                             totalCashSpent,
                                                             netWorth,
                                                             0.0}); // percentageGain is 0 since it is calculated during post-processing
}

// for now we set it as one time step = 1 month
int main()
{

    // for concurrent
    std::vector<SimulationInputsAndResults> simParametersVec_Con;

    // mutex for synchronisation of vectors when multiple threads are trying to
    // write to the vectors at the same time
    std::mutex simulationMutex;

    // concurrent approach
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::vector<std::jthread> threadPool;
    for (int i = 0; i < 100; i++)
    {
        threadPool.emplace_back(threadRunSim, 1900 + i, 1, 36, 0.0,
                                std::ref(simParametersVec_Con),
                                std::ref(simulationMutex));
    }
    for (auto &thread : threadPool)
    {
        thread.join();
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::string printout = std::format("concurrent simulation finished in {:>10} seconds", duration.count());
    std::cout << printout << std::endl;

    // for concurrent
    //  calculate percentage gain for each simulation, and store in percentageGainVec
    // std::transform(totalCashSpentVec_Con.begin(),
    //                totalCashSpentVec_Con.end(),
    //                netWorthVec_Con.begin(),
    //                std::back_inserter(percentageGainVec_Con),
    //                [](double x, double y)
    //                {
    //                    return (y - x) / x * 100.0;
    //                });
    for (auto &simResult : simParametersVec_Con)
    {
        simResult.percentageGain = (simResult.netWorth - simResult.totalCashSpent) / simResult.totalCashSpent * 100.0;
    }

    // formatting of header printout
    std::cout << std::format(
        "{:<15}|{:<18}|{:<22}|{:<18}|{:<15}|{:<15}\n",
        "Start(MM/YYYY)",
        "Duration(months)",
        "Initial Investment($)",
        "Cash Spent($)",
        "Net Worth($)",
        "% Gain");

    for (int i = 0; i < simParametersVec_Con.size(); ++i)
    {
        std::cout << std::format(
            "{:<15}|{:<18}|{:<22}|{:<18}|{:<15.2f}|{:<15.5f}\n",
            std::format("{}/{}", simParametersVec_Con[i].startMonth, simParametersVec_Con[i].startYear),
            simParametersVec_Con[i].durationMonths,
            simParametersVec_Con[i].initialInvestment,
            simParametersVec_Con[i].totalCashSpent,
            simParametersVec_Con[i].netWorth,
            simParametersVec_Con[i].percentageGain);
    }

    // for sequential (commented off for now since we are focusing on concurrent approach first)
    // std::vector<SimulationInputsAndResults> simParametersVec_Seq;
    // // sequential approach
    // std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < 100; ++i)
    // {
    //     threadRunSim(1900 + i, 1, 36, 0.0,
    //                  simParametersVec_Seq,
    //                  simulationMutex);
    // }
    // std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    // std::string printout = std::format("sequential simulation finished in {:>10} seconds", duration.count());
    // std::cout << printout << std::endl;

    // // for sequential
    // //  calculate percentage gain for each simulation, and store in percentageGainVec
    // // std::transform(totalCashSpentVec_Seq.begin(),
    // //                totalCashSpentVec_Seq.end(),
    // //                netWorthVec_Seq.begin(),
    // //                std::back_inserter(percentageGainVec_Seq),
    // //                [](double x, double y)
    // //                {
    // //                    return (y - x) / x * 100.0;
    // //                });
    // for (auto &simResult : simParametersVec_Seq)
    // {
    //     simResult.percentageGain = (simResult.netWorth - simResult.totalCashSpent) / simResult.totalCashSpent * 100.0;
    // }

    // // formatting of header printout
    // std::cout << std::format(
    //     "{:<15}|{:<18}|{:<22}|{:<18}|{:<15}|{:<15}\n",
    //     "Start(MM/YYYY)",
    //     "Duration(months)",
    //     "Initial Investment($)",
    //     "Cash Spent($)",
    //     "Net Worth($)",
    //     "% Gain");

    // for (int i = 0; i < simParametersVec_Seq.size(); ++i)
    // {
    //     std::cout << std::format(
    //         "{:<15}|{:<18}|{:<22}|{:<18}|{:<15.2f}|{:<15.5f}\n",
    //         std::format("{}/{}", simParametersVec_Seq[i].startMonth, simParametersVec_Seq[i].startYear),
    //         simParametersVec_Seq[i].durationMonths,
    //         simParametersVec_Seq[i].initialInvestment,
    //         simParametersVec_Seq[i].totalCashSpent,
    //         simParametersVec_Seq[i].netWorth,
    //         simParametersVec_Seq[i].percentageGain);
    // }
    // std::cout << std::endl;

    return 0;
}