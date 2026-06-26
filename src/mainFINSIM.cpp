#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>
#include <future>
#include <format>
#include <string>
#include <algorithm>
#include <ranges>
#include <cmath>
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
struct statsGroupedByDuration
{
    int durationMonths;
    double meanPercentageGain;
    double medianPercentageGain;
    double stdDevPercentageGain;
    double minPercentageGain;
    double maxPercentageGain;
};

std::tuple<double, double> runSimulation(const int startYear,
                                         const int startMonth,
                                         const int durationMonths,
                                         const double startFunds)
{
    // int cpuinit = sched_getcpu();
    // to do: add error handling if duration causes time to exceed stock price data available.
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

// this function is to allow us to dictate what parameters we want for the simulations,
// and to create the threads for the concurrent approach, in a more modular way
void threadCreator(std::vector<SimulationInputsAndResults> &simParametersVec_Con,
                   std::mutex &simulationMutex,
                   std::vector<std::jthread> &threadPool)
{
    int dataStartYear = 1871;                                        // csv data starts from Jan 1871
    int dataEndYear = 2025;                                          // we want data up to December, for simplicity
    const std::vector<int> durationVec{1, 2 /*, 3, 5, 10, 20, 30*/}; // duration in years
    std::vector<int> endYearVec(durationVec.size(), 0);
    for (int i = 0; i < durationVec.size(); i++)
    {
        endYearVec[i] = dataEndYear - durationVec[i]; // since we want to end the simulation at dataEndYear, and the last sim
                                                      // commences at the endYear, so need to make sure it does not overshoot the data from csv
    }
    for (int i = dataStartYear; i <= dataEndYear; i++)
    {
        for (int j = 0; j < endYearVec.size(); j++)
        {
            if (i <= endYearVec[j])
            {
                for (int k = 1; k <= 12; k++) // start sim for every month in each year
                {
                    threadPool.emplace_back(threadRunSim, i, k, durationVec[j] * 12, 0.0,
                                            std::ref(simParametersVec_Con),
                                            std::ref(simulationMutex));
                }
            }
        }
    }
}

// for post-processing of simulation results
statsGroupedByDuration calculateStats(std::vector<SimulationInputsAndResults>::iterator startIt,
                                      std::vector<SimulationInputsAndResults>::iterator endIt)
{
    statsGroupedByDuration stats{};
    stats.durationMonths = startIt->durationMonths;

    const size_t numResults = std::distance(startIt, endIt);

    // calculate mean
    double sum = 0.0;
    for (auto it = startIt; it != endIt; ++it)
    {
        sum += it->percentageGain;
    }

    stats.meanPercentageGain = sum / numResults;

    // calculate median
    std::vector<double> percentageGains;
    percentageGains.reserve(numResults);
    for (auto it = startIt; it != endIt; ++it)
    {
        percentageGains.push_back(it->percentageGain);
    }
    std::ranges::sort(percentageGains);
    if (numResults % 2 == 0)
    {
        stats.medianPercentageGain = (percentageGains[numResults / 2 - 1] + percentageGains[numResults / 2]) / 2.0;
    }
    else
    {
        stats.medianPercentageGain = percentageGains[numResults / 2];
    }

    // calculate standard deviation
    double variance = 0.0;
    for (auto it = startIt; it != endIt; ++it)
    {
        double difference = it->percentageGain - stats.meanPercentageGain;
        variance += difference * difference;
    }
    variance /= numResults;
    stats.stdDevPercentageGain = std::sqrt(variance);

    // calculate min and max
    auto [minIt, maxIt] = std::minmax_element(percentageGains.begin(), percentageGains.end());
    stats.minPercentageGain = *minIt;
    stats.maxPercentageGain = *maxIt;

    return stats;
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
    threadCreator(simParametersVec_Con, simulationMutex, threadPool);
    for (auto &thread : threadPool)
    {
        thread.join();
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::string printout = std::format("concurrent simulation finished in {:>10} seconds, with {} simulation runs", duration.count(), simParametersVec_Con.size());
    std::cout << printout << std::endl;

    for (auto &simResult : simParametersVec_Con)
    {
        simResult.percentageGain = (simResult.netWorth - simResult.totalCashSpent) / simResult.totalCashSpent * 100.0;
    }

    // sort results by duration, ascending order
    std::ranges::sort(simParametersVec_Con, {}, &SimulationInputsAndResults::durationMonths); // sort by default order, using duration as the key

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


    return 0;
}