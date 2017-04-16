#include "Concurrency.hpp"
#include "Log.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <future>

using namespace std::chrono;
using namespace std::chrono_literals;
using cclock = steady_clock;

constexpr size_t maxTestCount = 10000000;
size_t testCount = 1;

class MyLogger :public Carbon::Logger {
private:
    std::string format(const char* func , const char* file , std::int32_t line , std::int32_t sev , 
        const std::string& str) const override {
        return "Logger :"+str;
    }
    void flush(const std::string& buffer) override {
        std::cout << buffer<<std::endl;
    }
} logger;
auto obj = [] () {
    int c = 0;
    for (int i = 0; i < 100; ++i)
        c += rand();
    return c;
};

auto test() {
    std::vector<std::future<int>> futs; futs.reserve(testCount);
    std::vector<int> result(testCount);
    auto start = cclock::now();
    for (size_t i = 0; i < testCount; ++i)
        result[i] = obj();
    return (cclock::now() - start) / 1us;
}

auto testOMP() {
    std::vector<std::future<int>> futs; futs.reserve(testCount);
    std::vector<int> result(testCount);
    auto start = cclock::now();

#pragma omp parallel for
    for (int i = 0; i < testCount; ++i)
        result[i] = obj();
    return (cclock::now() - start) / 1us;
}

auto testThreadPool() {

    Carbon::ThreadPool pool {};
    std::vector<std::future<int>> futs; futs.reserve(testCount);
    auto start = cclock::now();
    for (size_t i = 0; i < testCount; ++i)
        futs.push_back(Carbon::Async(pool , obj));
    for (auto&& a : futs)
        a.wait();
    return (cclock::now() - start) / 1us;
}

auto testThreadPoolTaskGroup() {

    Carbon::ThreadPool pool {};
    std::vector<int> result(testCount);
    auto start = cclock::now();
    using Range = Carbon::TaskGroupHelper::IntegerRange;
    std::function<void(Range)> func = Range::forEach([&] (size_t i) {result[i] = obj(); });
    auto future = Carbon::AsyncGroup(pool , func ,
        Carbon::TaskGroupHelper::IntegerRange { 0,testCount });
    future->wait();
    return (cclock::now() - start) / 1us;
}


void sleepTest() {
    Carbon::ThreadPool pool {};
    std::this_thread::sleep_for(1s);
}

int main() {
    CARBON_LOG_SEV(logger , 0) << "Hello Logger!";

    sleepTest();

    while (testCount<=maxTestCount) {
        std::cout << "Async Benchmark for Small Tasks:" << testCount << "ops" << std::endl;
        float t0;
        long long t;
        t0 = t = test();
        t0 *= 100.0f;
        std::cout.precision(2);
        std::cout << std::fixed;
        std::cout << "Default:" << t << "us " << 100.0f << "%" << std::endl;
        t = testOMP();
        std::cout << "OpenMP:" << t << "us " << t0 / t << "%" << std::endl;
        t = testThreadPool();
        std::cout << "ThreadPool:" << t << "us " << t0 / t << "%" << std::endl;
        t = testThreadPoolTaskGroup();
        std::cout << "ThreadPool with TaskGroup:" << t << "us " << t0 / t << "%" << std::endl;
        testCount *= 10;
    }
    system("pause");
    return 0;
}
