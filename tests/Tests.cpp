#include UNITTESTINC
#include "Concurrency.hpp"
#include "Log.hpp"
#include <vector>
#include <future>

namespace CarbonTests {
    namespace ConcurrencyTests {
        constexpr size_t maxTestCount = 10000000;
        auto obj = []() {
            int c = 0;
            for (int i = 0; i < 100; ++i)
                c += rand();
            return c;
        };

        BEGIN_TEST_GROUP(Concurrency)
            size_t testCount = 1;
        void test() {
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            std::vector<int> result(testCount);
            for (size_t i = 0; i < testCount; ++i)
                result[i] = obj();
        }

        void testOMP() {
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            std::vector<int> result(testCount);
#pragma omp parallel for
            for (int i = 0; i < testCount; ++i)
                result[i] = obj();
        }

        void testThreadPool() {
            Carbon::ThreadPool pool{};
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            for (size_t i = 0; i < testCount; ++i)
                futs.push_back(Carbon::Async(pool, obj));
            for (auto&& a : futs)
                a.wait();
        }

        void testThreadPoolTaskGroup() {
            Carbon::ThreadPool pool{};
            std::vector<int> result(testCount);
            using Range = Carbon::TaskGroupHelper::IntegerRange;
            std::function<void(Range)> func = Range::forEach([&](size_t i) {result[i] = obj(); });
            auto future = Carbon::AsyncGroup(pool, func,
                Carbon::TaskGroupHelper::IntegerRange{ 0,testCount });
            future->wait();
        }
        END_TEST_GROUP
    }
}
