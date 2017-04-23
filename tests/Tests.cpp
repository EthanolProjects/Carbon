#include UNITTESTINC
#include "Concurrency.hpp"
#include "Log.hpp"
#include <vector>
#include <future>

namespace CarbonTests {
    namespace ConcurrencyTests {

        auto obj = [] () {
            int c = 0;
            for (int i = 0; i < 1000; ++i)
                c += rand();
            return c;
        };

        void useResult(int v) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(v % 10));
        }
        int getPos(int size) {
            return size*(1 - static_cast<float>(rand()) / RAND_MAX);
        }

        BEGIN_TEST_GROUP(Concurrency)
#define ETH_TEST_SUITE Concurrency

            void testThreadPool(size_t testCount) {
            Carbon::ThreadPool pool {};
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            std::atomic_size_t count { 0 };
            for (size_t i = 0; i < testCount; ++i)
                futs.push_back(Carbon::Async(pool, [&] {
                ++count;
                return obj(); }));
            for (auto&& a : futs)
                a.wait();
            useResult(futs[getPos(testCount)].get());
            ASSERT_EQ(testCount, static_cast<size_t>(count));
        }

        void testThreadPoolTaskGroup(size_t testCount) {
            Carbon::ThreadPool pool {};
            std::vector<int> result(testCount);
            std::atomic_size_t count { 0 };
            using Range = Carbon::TaskGroupHelper::IntegerRange;
            std::function<void(Range)> func = Range::forEach([&] (size_t i) {
                ++count;
                result[i] = obj();
            });
            auto future = Carbon::AsyncGroup(pool, func, { 0,testCount },1024);
            future->wait();
            useResult(result[getPos(testCount)]);
            ASSERT_EQ(testCount, static_cast<size_t>(count));
        }

#define COR_TEST_NUM(name,id,num)\
    TEST_METHOD(name##CorrectnessTest##id){\
        try{\
            test##name(num);\
        }\
        catch(...) {FAIL();}\
    }

#define COR_TEST(name) \
        COR_TEST_NUM(name,1,1)\
        COR_TEST_NUM(name,2,10)\
        COR_TEST_NUM(name,3,100)\
        COR_TEST_NUM(name,4,1000)\
        COR_TEST_NUM(name,5,10000)\
        COR_TEST_NUM(name,6,100000)\
        COR_TEST_NUM(name,7,1000000)\
        COR_TEST_NUM(name,8,10000000)
        COR_TEST(ThreadPool);
        COR_TEST(ThreadPoolTaskGroup);

        static constexpr size_t maxNum = 1000000;
        TEST_METHOD(ThreadPoolExtremalTest) {
            using namespace std::literals;
            Carbon::ThreadPool A {}, B {};
            using Range = Carbon::TaskGroupHelper::IntegerRange;
            std::vector<int> result(maxNum);
            std::function<void(Range)> func1 =
                Range::forEach([&] (size_t i) {result[i] = obj(); });
            std::function<void(Range)> func2 = [&] (Range range) {
                if (rand() > RAND_MAX / 2) {
                    std::vector<std::future<void>> v; v.reserve(range.size());
                    Range::forEach([&] (size_t i) {
                        v.push_back(Carbon::Async(B, [&, i] {result[i] = obj(); }));
                    })(range);
                    for (auto&& x : v) x.wait();
                }
                else Carbon::AsyncGroup(B, func1, range)->wait();
            };
            Carbon::AsyncGroup(A, func2, { 0,maxNum }, 128)->wait();
            useResult(result[getPos(maxNum)]);
        }

        TEST_METHOD(PerfTestOMP) {
            std::vector<int> result(maxNum);
#          pragma omp parallel for
            for (int i = 0; i < maxNum; ++i)
                result[i] = obj();
            useResult(result[getPos(maxNum)]);
        }

        TEST_METHOD(PerfTestCTP) {
            std::vector<int> result(maxNum);
            Carbon::ThreadPool pool {};
            using Range = Carbon::TaskGroupHelper::IntegerRange;
            std::function<void(Range)> func = Range::forEach([&] (size_t i) {result[i] = obj(); });
            auto future = Carbon::AsyncGroup(pool, func, { 0,maxNum });
            future->wait();
            useResult(result[getPos(maxNum)]);
        }

#undef ETH_TEST_SUITE
        END_TEST_GROUP
    }
}
RUN_TESTS
