#include "Carbon/System/Concurrency.hpp"
#include <vector>
#include <cassert>
#include <iostream>

namespace CarbonTests {
    auto obj() {
        int c = 0;
        for (int i = 1; i <= 1000; ++i)
            c += rand() % i;
        return c;
    };
    auto obj2() {
        return rand();
    };

    void testThreadPool(size_t testCount) {
        std::vector<Carbon::Promise<int>> futs; futs.reserve(testCount);
        std::atomic_size_t count{ 0 };
        for (size_t i = 0; i < testCount; ++i)
            futs.push_back(Carbon::async([&] { ++count; return obj(); }));
        for (auto&& a : futs)
            a.get();
    }

    void testThreadPoolForEach(size_t testCount) {
        std::atomic_int count{ 0 };
        Carbon::asyncForRange(size_t(0), testCount, [&](size_t) { ++count; return obj(); }).get();
        if (count != testCount) {
            std::cout << "Expected:" << testCount << " Actual:" << count << std::endl;
        }
    }

}

int main() {
    for (int i = 0; i < 100; ++i) {
        CarbonTests::testThreadPoolForEach(1000);
        std::cout << i << std::endl;
    }
    system("pause");
}