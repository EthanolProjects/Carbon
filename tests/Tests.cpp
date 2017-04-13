#include "Concurrency.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <future>
using namespace std::chrono;
using namespace std::chrono_literals;
using cclock = steady_clock;

constexpr int testCount = 300000;

auto obj = []() {
    return 1;
};

int testAsync(std::launch pol) {
    std::vector<std::future<int>> futs; futs.reserve(testCount);
    auto start = cclock::now();
    for (size_t i = 0; i < testCount; ++i)
        futs.push_back(std::async(pol, obj));
    for (auto&& a : futs)
        a.wait();
    return (cclock::now() - start) / 1us;
}

int testThreadPool() {
    //static 
        Carbon::ThreadPool pool{};
    std::vector<std::future<int>> futs; futs.reserve(testCount);
    auto start = cclock::now();
    for (size_t i = 0; i < testCount; ++i)
        futs.push_back(Async(pool, obj));
    for (auto&& a : futs)
        a.wait();
    return (cclock::now() - start) / 1us;
}

int main() {
    std::cout << "Async Benchmark for Small Tasks:" << testCount << "ops" << std::endl;
    std::cout << "Default|Aync|Deferred|ThreadPool" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        std::cout << testAsync(std::launch::async | std::launch::deferred) << '|' <<
            testAsync(std::launch::async) << '|' << testAsync(std::launch::deferred) <<
            '|' << testThreadPool() << std::endl;
    }
    system("pause");
}