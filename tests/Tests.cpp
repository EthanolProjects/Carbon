#include UNITTESTINC
#include <vector>
#include <future>
#include "Concurrency.hpp"
namespace CarbonTests {
    auto obj = []() {
        return 1;
    };
    constexpr int testCount = 300000;
    BEGIN_TEST_GROUP(Concurrency)
    void async() {
        std::vector<std::future<int>> futs; futs.reserve(testCount);
        for (size_t i = 0; i < testCount; ++i)
            futs.push_back(std::async(obj));
        for (auto&& a : futs)
            a.wait();
    }

    void threadPool() {
        Carbon::ThreadPool pool{};
        std::vector<std::future<int>> futs; futs.reserve(testCount);
        for (size_t i = 0; i < testCount; ++i)
            futs.push_back(Async(pool, obj));
        for (auto&& a : futs)
            a.wait();
    }
    TEST_METHOD(TestSTDAsync) { async(); }
    TEST_METHOD(TestCarbonThreadPool) { threadPool(); }
    END_TEST_GROUP
}
