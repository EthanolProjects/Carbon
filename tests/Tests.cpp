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
    void flush(const std::stringstream& buffer) override {
        std::cerr << buffer.rdbuf();
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

struct Range {
    size_t begin , end;
    Range cut(size_t atomic) {
        auto lb = begin;
        begin += atomic;
        if (begin > end)begin = end;
        return { lb,begin };
    }
    size_t size() const {
        return end - begin;
    }
};

auto testThreadPoolTaskGroup() {

    Carbon::ThreadPool pool {};
    std::vector<int> result(testCount);
    auto start = cclock::now();
    std::function<void(Range)> func = [&] (Range range) {
        for (size_t i = range.begin; i < range.end; ++i) {
            result[i] = obj();
        }
    };
    auto future = Carbon::AsyncGroup(pool , func , Range { 0,testCount });
    future->wait();
    return (cclock::now() - start) / 1us;
}

int main() {
    CARBON_LOG_SEV(logger , 0) << "Hello Logger!";

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
