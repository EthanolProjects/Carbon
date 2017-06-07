#include UNITTESTINC
#include "Carbon/Concurrency/Concurrency.hpp"
#include "Carbon/Math/Math.hpp"
#include <vector>
#include <future>

namespace CarbonTests {
    namespace ConcurrencyTests {
        auto obj(){
            int c = 0;
            for (int i = 1; i <= 1000; ++i)
                c += rand()%i;
            return c;
        };
        auto obj2(){
            return rand();
        };

#define ETH_TEST_SUITE Concurrency
        BEGIN_TEST_GROUP

        void testThreadPool(size_t testCount) {
            auto pool = Carbon::Threadpool::create();
            std::vector<std::future<int>> futs; futs.reserve(testCount);
            std::atomic_size_t count{ 0 };
            for (size_t i = 0; i < testCount; ++i)
                futs.push_back(Carbon::async(*pool, [&] { ++count; return obj(); }));
            for (auto&& a : futs)
                a.get();
            ASSERT_EQ(testCount, static_cast<size_t>(count));
        }

        void testThreadPoolTaskGroup(size_t testCount) {
            auto pool = Carbon::Threadpool::create();
            std::atomic_size_t count{ 0 };
            auto func = [&](size_t i) { ++count; obj(); };
            Carbon::asyncForIntegerRange(*pool, func, 0, testCount).get();
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

        static constexpr size_t maxNum = 1000000;

        TEST_METHOD(PerfTestOMP) {
            for (int i = 0; i < maxNum; ++i) obj2();
        }

        TEST_METHOD(PerfTestCTP) {
            auto pool = Carbon::Threadpool::create();
            auto func = [&](size_t i) { obj2(); };
            Carbon::asyncForIntegerRange(*pool, func, 0, maxNum).get();
        }       

        END_TEST_GROUP
#undef ETH_TEST_SUITE
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
