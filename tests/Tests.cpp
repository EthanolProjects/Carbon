#include "Concurrency.hpp"
#include "Log.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <future>
using namespace std::chrono;
using namespace std::chrono_literals;
using cclock = steady_clock;

constexpr int testCount = 100000;
class MyLogger :public Carbon::Logger{
private:
    void flush(const std::stringstream& buffer) override
    {
        std::cerr << buffer.rdbuf();
    }
} logger;
auto obj = []() {
    int c=0;
    for (int i = 0; i < 1000; ++i)
        c += rand();
    return c;
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
        CARBON_LOG_SEV(logger , 0) << "Default:" << testAsync(std::launch::async | std::launch::deferred);
        CARBON_LOG_SEV(logger , 0) << "Async:" << testAsync(std::launch::async);
        CARBON_LOG_SEV(logger , 0) << "Deferred:" << testAsync(std::launch::deferred);
        CARBON_LOG_SEV(logger , 0) << "ThreadPool:" << testThreadPool();
    }
    system("pause");
}