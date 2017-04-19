#include UNITTESTINC
#include "Concurrency.hpp"
#include "Log.hpp"
#include <vector>
#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
        void testThreadPool() {
            Carbon::ThreadPool pool{};
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            std::atomic_size_t count=0;
            for (size_t i = 0; i < testCount; ++i)
                futs.push_back(Carbon::Async(pool , [&] {++count; return obj(); }));
            for (auto&& a : futs)
                a.wait();
            Assert::AreEqual(testCount , static_cast<size_t>(count) , nullptr , LINE_INFO());
        }

        void testThreadPoolTaskGroup() {
            Carbon::ThreadPool pool{};
            std::vector<int> result(testCount);
            std::atomic_size_t count = 0;
            using Range = Carbon::TaskGroupHelper::IntegerRange;
            std::function<void(Range)> func = Range::forEach([&] (size_t i) {result[i] = obj(); ++count; });
            auto future = Carbon::AsyncGroup(pool, func,
                Carbon::TaskGroupHelper::IntegerRange{ 0,testCount });
            future->wait();
            Assert::AreEqual(testCount , static_cast<size_t>(count) , nullptr , LINE_INFO());
        }
        TEST_METHOD(ConcurrencyTests)
        {
            while (testCount<=100000) {
                testThreadPool();
                testThreadPoolTaskGroup();
                testCount *= 10;
            }
        }
        END_TEST_GROUP
    }
}
