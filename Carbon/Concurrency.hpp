#pragma once
#include "Config.hpp"
#include "Mpl.hpp"
#include <memory>
#include <future>

namespace Carbon {
    class CARBON_API Work {
    public:
        virtual void main() noexcept = 0;
    };

    class CARBON_API Threadpool {
    public:
        virtual ~Threadpool() {}
        virtual void submitOnce(Work* task) = 0;
        virtual size_t getConcurrencyLevel() const;
        static Threadpool& getDefault() noexcept;
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
            void main() noexcept override {
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
            WorkIntegerRange(Callable call, int spawn, int begin, int end) :
                mCallable(std::forward<Callable>(call)), mRange(begin, end), mCurrent{ begin }, mCount(spawn) {}
            void main() noexcept override {
                try {
                    int cutCurrent;
                    for (; (cutCurrent = mCurrent.fetch_add(1)) < mRange.second;)
                        mCallable(cutCurrent);
                }
                catch (...) {
                    if (!mExpFlag.test_and_set()) {
                        mCurrent.store(mRange.second); // Break the execution
                        mPromise.set_exception(std::current_exception());
                    }
                }
                if (mCount.fetch_sub(1) == 1) {
                    if (!mExpFlag.test_and_set())
                        mPromise.set_value();
                    delete this;
                }
            }
            auto getFuture() { return mPromise.get_future(); }
        private:
            Callable mCallable;
            std::pair<int, int> mRange;
            std::promise<void> mPromise;
            std::atomic_int mCurrent, mCount;
            std::atomic_flag mExpFlag{ ATOMIC_FLAG_INIT };
        };
    }

    template<class Callable, class ...Ts>
    inline auto async(Threadpool& pool, const Callable& callable, Ts&&... args) {
        auto newTask = new TppDetail::TaskFunc<Callable, Ts...>(callable, std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        pool.submitOnce(newTask);
        return fut;
    }

    template <class Callable>
    inline auto asyncForIntegerRange(Threadpool& pool, const Callable& callable, int begin, int end) {
        auto spawn = pool.getConcurrencyLevel();
        auto newTask = new TppDetail::WorkIntegerRange<Callable>(callable, spawn, begin, end);
        auto fut = newTask->getFuture();
        for (int i = 0; i < spawn; ++i) pool.submitOnce(newTask);
        return fut;
    }
}
