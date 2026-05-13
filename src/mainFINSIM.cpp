#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <future>
#include "Cash.h"
#include "Stock.h"

// for linux
#include <sched.h>

void runSimulation(std::mutex &coutMtx,
                   int startYear,
                   int startMonth,
                   int durationMonths,
                   int startFunds)
{
    int cpuinit = sched_getcpu();
    Stock snp500("../data/SNP500_monthly_price_1871_to_Mar_2026.csv", startYear, startMonth); // ../ since directory is in build folder, so need to go back one folder
    Cash bankAccount(startFunds);                                                             // start with $0
    const double salary = 200.0;
    double totalCashSpent = 0.0;
    for (int i = 0; i < durationMonths; i++)
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
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    int cpuend = sched_getcpu();
    {
        std::lock_guard<std::mutex> lock(coutMtx);
        std::cout << "simulation started on CPU " << cpuinit << " and ended on CPU " << cpuend << std::endl;
    }
}

// for now we set it as one time step = 1 month
int main()
{
    std::mutex coutMtx_;

    // concurrent runing of 10 simulations
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++)
    {
        runSimulation(coutMtx_, 1973, 11, 36, 0.0);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "simulation finished in " << duration.count() << std::endl;

    // concurrent running of 10 simulations using threads
    // std::cin.get();
    start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threadPool;
    for (int i = 0; i < 10; i++)
    {
        threadPool.emplace_back(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    }
    for (auto &thread : threadPool)
    {
        thread.join();
    }

    // std::thread t1(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t2(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t3(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t4(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t5(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t6(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t7(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t8(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t9(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    // std::thread t10(runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);

    // t1.join();
    // t2.join();
    // t3.join();
    // t4.join();
    // t5.join();
    // t6.join();
    // t7.join();
    // t8.join();
    // t9.join();
    // t10.join();

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "simulation finished in " << duration.count() << std::endl;

    // concurrent running of 10 simulations using async
    start = std::chrono::high_resolution_clock::now();
    auto t1async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t2async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t3async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t4async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t5async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t6async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t7async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t8async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t9async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);
    auto t10async = std::async(std::launch::async, runSimulation, std::ref(coutMtx_), 1973, 11, 36, 0.0);

    t1async.get();
    t2async.get();
    t3async.get();
    t4async.get();
    t5async.get();
    t6async.get();
    t7async.get();
    t8async.get();
    t9async.get();
    t10async.get();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "simulation finished in " << duration.count() << std::endl;

    return 0;
}