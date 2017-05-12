#pragma once
#include "Config.hpp"
#include "Mpl.hpp"
#include <thread>
#include <memory>
#include <vector>
#include <future>

namespace Carbon {
    class CARBON_API Work {
    public:
        virtual void execute() = 0;
    };

    class CARBON_API Threadpool {
    public:
        virtual ~Threadpool() {}
        virtual void submit(Work* task) = 0;
        virtual size_t getConcurrencyLevel() const { return std::thread::hardware_concurrency(); }
        static Threadpool& default() noexcept;
        static std::unique_ptr<Threadpool> create();
    };

    namespace TppDetail {
        template<typename T, class Callable, class ...Ts>
        struct ApplyImpl {
            static void doAndSet(std::promise<T>& promise, Callable& callable, std::tuple<Ts...>& tuple) {
                promise.set_value(Apply(callable, tuple));
            }
        };

        template<class Callable, class ...Ts>
        struct ApplyImpl<void, Callable, Ts...> {
            static void doAndSet(std::promise<void>& promise, Callable& callable, std::tuple<Ts...>& tuple) {
                Apply(callable, tuple);
                promise.set_value();
            }
        };
        
        template <class Callable, class ...Ts>
        class TaskFunc :public Work {
            using ReturnType =
                std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>;
        public:
            TaskFunc(Callable call, Ts&&... args) :
                mCallable(std::forward<Callable>(call)), mTuple(std::forward_as_tuple(args...)) {}
            void execute() override {
                try {
                    ApplyImpl<ReturnType, Callable, Ts...>::doAndSet(mPromise, mCallable, mTuple);
                }
                catch (...) {
                    mPromise.set_exception(std::current_exception());
                }
                delete this;
            }
            auto getFuture() { return mPromise.get_future(); }
        private:
            Callable mCallable;
            std::tuple<Ts...> mTuple;
            std::promise<ReturnType> mPromise;
        };

        template<class Callable>
        class WorkIntegerRange: public Work {
        public:
            WorkIntegerRange(Callable call, int begin, int end, int step) :
                mCallable(std::forward<Callable>(call)), mRange(begin, end), mCurrent{ begin } {}
            void execute() override {
                ++mCount;
                try {
                    int cutCurrent;
                    for (; (cutCurrent = mCurrent++) < mRange.second;)
                        mCallable(cutCurrent);
                }
                catch (...) {
                    mPromise.set_exception(std::current_exception());
                }
                if (--mCount == 0) {
                    mPromise.set_value();
                    delete this;
                }
            }
            auto getFuture() { return mPromise.get_future(); }
        private:
            Callable mCallable;
            std::pair<int, int> mRange;
            std::promise<void> mPromise;
            std::atomic_int mCurrent, mCount{ 0 };
        };
    }

    template<class Callable, class ...Ts>
    inline auto async(Threadpool& pool, const Callable& callable, Ts&&... args) {
        auto newTask = new TppDetail::TaskFunc<Callable, Ts...>(callable, std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        pool.submit(newTask);
        return fut;
    }

    template <class Callable>
    inline auto asyncForIntegerRange(Threadpool& pool, const Callable& callable, int begin, int end, int step = 1) {
        auto newTask = new TppDetail::WorkIntegerRange<Callable>(callable, begin, end, step);
        auto fut = newTask->getFuture();
        for (int i = 0; i < pool.getConcurrencyLevel(); ++i) pool.submit(newTask);
        return fut;
    }
}
