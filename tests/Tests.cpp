#include "ThreadPool.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <future>
constexpr int testCount = 300000;
double testAsync(std::launch pol) {
    auto obj = []() {
        return 1;
    };
    using namespace std;
    using namespace chrono_literals;
    using namespace chrono;
    using clock = steady_clock;
    std::vector<decltype(std::async(obj))> futs; futs.reserve(testCount);
    auto start = clock::now();
    for (size_t i = 0; i < testCount; ++i)
        futs.push_back(std::async(pol, obj));
    for (auto&& a : futs)
        a.wait();
    return (clock::now() - start) / 1us;
}

double testThreadPool() {
    using namespace Carbon;
    static ThreadPool pool{0};
    auto obj = []() {
        return 1;
    };
    using namespace std;
    using namespace chrono_literals;
    using namespace chrono;
    using clock = steady_clock;
    std::vector<decltype(Async(pool, obj))> futs; futs.reserve(testCount);
    auto start = clock::now();
    for (size_t i = 0; i < testCount; ++i)
        futs.push_back(Async(pool, obj));
    for (auto&& a : futs)
        a.wait();
    auto diff = clock::now() - start;
    return (clock::now() - start) / 1us;
}

int main() {
    std::cout << "Async Benchmark for Small Tasks:" << testCount << "ops" << std::endl;
    std::cout << "Default|Aync|Deferred|ThreadPool" << std::endl;
    for (size_t i = 0; i < 20; ++i) {
        std::cout << testAsync(std::launch::async | std::launch::deferred) << '|' <<
            testAsync(std::launch::async) << '|' << testAsync(std::launch::deferred) <<
            '|' << testThreadPool() << std::endl;
    }
    system("pause");
}