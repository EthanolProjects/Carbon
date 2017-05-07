#include UNITTESTINC
#include "Concurrency.hpp"
#include "Math.hpp"
#include "Log.hpp"
#include <vector>
#include <future>

namespace CarbonTests {
    namespace ConcurrencyTests {
        auto obj = []{
            int c = 0;
            for (int i = 1; i <= 1000; ++i)
                c += rand()%i;
            return c;
        };
        auto obj2 = []{
            return rand();
        };

#define ETH_TEST_SUITE Concurrency
        BEGIN_TEST_GROUP

        void testThreadPool(size_t testCount) {
            auto& pool = Carbon::Threadpool::default();
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            std::atomic_size_t count{ 0 };
            for (size_t i = 0; i < testCount; ++i)
                futs.push_back(Carbon::Async(pool, [&] {
                ++count;
                return obj(); }));
            for (auto&& a : futs)
                a.wait();
            ASSERT_EQ(testCount, static_cast<size_t>(count));
        }

        void testThreadPoolTaskGroup(size_t testCount) {
            auto& pool = Carbon::Threadpool::default();
            std::vector<int> result(testCount);
            std::atomic_size_t count{ 0 };
            auto func = [&](size_t i) {
                ++count;
                result[i] = obj();
            };
            auto future = Carbon::AsyncGroup(pool, { 0,testCount }, func, 1024);
            future->wait();
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
        COR_TEST_NUM(name,4,1000)
        COR_TEST(ThreadPool);
        COR_TEST(ThreadPoolTaskGroup);

        static constexpr size_t maxNum = 10000000;

        TEST_METHOD(ThreadPoolExtremalTest) {
            using namespace std::literals;
            auto A = Carbon::Threadpool::create();
            auto B = Carbon::Threadpool::create();
            using Range = Carbon::IntegerRange;
            std::vector<int> result(maxNum);
            auto func1 = [&](size_t i) {result[i] = obj2(); };
            auto func2 = [&](Range range) {
                if (rand() > RAND_MAX / 2) {
                    std::vector<std::future<void>> v; v.reserve(range.size());
                    range.forEach([&](size_t i) {
                        v.push_back(Carbon::Async(*B, [&, i] {result[i] = obj2(); }));
                    });
                    for (auto&& x : v) x.wait();
                }
                else Carbon::AsyncGroup(*B, range, func1)->wait();
            };
            Carbon::AsyncGroup(*A, { 0,maxNum }, func2, 128)->wait();
        }

        TEST_METHOD(PerfTestOMP) {
            std::vector<int> result(maxNum);
#pragma omp parallel for
            for (int i = 0; i < maxNum; ++i)
                result[i] = obj2();
        }
        

        TEST_METHOD(PerfTestCTP) {
            std::vector<int> result(maxNum);
            auto& pool = Carbon::Threadpool::default();
            auto func = [&](size_t i) {result[i] = obj2(); };
            auto future = Carbon::AsyncGroup(pool, { 0,maxNum }, func);
            future->wait();
        }       

#undef ETH_TEST_SUITE
        END_TEST_GROUP
    }
    namespace MathTests {
#define ETH_TEST_SUITE MathTests
        BEGIN_TEST_GROUP
        TEST_METHOD(Vec2iAdd) {
            using Vec = Carbon::Vec2i;
            ASSERT_TRUE(Vec(1, 1) + Vec(2, 2) == Vec(3, 3));
        }
        END_TEST_GROUP
#undef ETH_TEST_SUITE
    }
}
