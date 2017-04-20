#include UNITTESTINC
#include "Concurrency.hpp"
#include "Log.hpp"
#include <vector>
#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CarbonTests {
    namespace ConcurrencyTests {

        auto obj = []() {
            int c = 0;
            for (int i = 0; i < 100; ++i)
                c += rand();
            return c;
        };

        BEGIN_TEST_GROUP(Concurrency)
        void testThreadPool(size_t testCount) {
            Carbon::ThreadPool pool{};
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            std::atomic_size_t count=0;
            for (size_t i = 0; i < testCount; ++i)
                futs.push_back(Carbon::Async(pool , [&] {++count; return obj(); }));
            for (auto&& a : futs)
                a.wait();
            Assert::AreEqual(testCount , static_cast<size_t>(count) , nullptr , LINE_INFO());
        }

        void testThreadPoolTaskGroup(size_t testCount) {
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

		void baseLine(size_t testCount) {
			std::vector<std::future<int>> futs; futs.reserve(testCount);
			std::vector<int> result(testCount);
#pragma omp parallel for
			for (int i = 0; i < testCount; ++i)
				result[i] = obj();
		}
		TEST_METHOD(BaseLine01) { baseLine(1); }
		TEST_METHOD(BaseLine02) { baseLine(10); }
		TEST_METHOD(BaseLine03) { baseLine(100); }
		TEST_METHOD(BaseLine04) { baseLine(1000); }
		TEST_METHOD(BaseLine05) { baseLine(10000); }
		TEST_METHOD(BaseLine06) { baseLine(100000); }
		TEST_METHOD(BaseLine07) { baseLine(1000000); }
		TEST_METHOD(BaseLine08) { baseLine(10000000); }
		TEST_METHOD(ThreadPool01) { testThreadPool(1); }
		TEST_METHOD(ThreadPool02) { testThreadPool(10); }
		TEST_METHOD(ThreadPool03) { testThreadPool(100); }
		TEST_METHOD(ThreadPool04) { testThreadPool(1000); }
		TEST_METHOD(ThreadPool05) { testThreadPool(10000); }
		TEST_METHOD(ThreadPool06) { testThreadPool(100000); }
		TEST_METHOD(ThreadPoolTaskGroup01) { testThreadPoolTaskGroup(1); }
		TEST_METHOD(ThreadPoolTaskGroup02) { testThreadPoolTaskGroup(10); }
		TEST_METHOD(ThreadPoolTaskGroup03) { testThreadPoolTaskGroup(100); }
		TEST_METHOD(ThreadPoolTaskGroup04) { testThreadPoolTaskGroup(1000); }
		TEST_METHOD(ThreadPoolTaskGroup05) { testThreadPoolTaskGroup(10000); }
		TEST_METHOD(ThreadPoolTaskGroup06) { testThreadPoolTaskGroup(100000); }
		TEST_METHOD(ThreadPoolTaskGroup07) { testThreadPoolTaskGroup(1000000); }
		TEST_METHOD(ThreadPoolTaskGroup08) { testThreadPoolTaskGroup(10000000); }
        END_TEST_GROUP
    }
}
