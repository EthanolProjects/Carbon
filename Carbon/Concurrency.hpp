#pragma once
#include "Config.hpp"
#include "Mpl.hpp"
#include <thread>
#include <memory>
#include <vector>
#include <future>

namespace Carbon {
    class CARBON_API Task {
    public:
        virtual void execute() = 0;
        virtual bool reusable();
    };

    class CARBON_API ThreadPool final {
    public:
        ThreadPool();
        ThreadPool(size_t num);
        ~ThreadPool();
        void addTask(Task* task);
        size_t size() const;
    private:
        class TaskQueue;
        class ThreadGroup;
        size_t mSize;
        std::unique_ptr<TaskQueue> mSource;
        std::unique_ptr<ThreadGroup> mThreads;
    };

    namespace TppDetail {
        template<typename T, typename T2> class SubTask;
    }

    class CARBON_API TaskGroupFuture final {
    public:
        TaskGroupFuture(size_t size);
        ~TaskGroupFuture();
        void wait() const;
        template<class Clock, class Duration>
        bool wait_until(const std::chrono::time_point<Clock, Duration>& time) const {
            while (mLast) {
                std::this_thread::yield();
                if (std::chrono::system_clock::now() >= time)
                    return false;
            }
            return true;
        }
        template<class Rep, class Period>
        bool wait_for(const std::chrono::duration<Rep, Period>& time) const {
            return wait_until(std::chrono::system_clock::now() + time);
        }
        void catchExceptions(const std::function<void(std::function<void()>)>& catchFunc);
    private:
        template<typename T, typename T2>
        friend class TppDetail::SubTask;
        size_t finish(size_t size);
        size_t setException(std::exception_ptr exc, size_t size);
        std::atomic_size_t mLast;
        std::vector<std::exception_ptr> mExceptions;
        std::mutex mMutex;
    };

    namespace TppDetail {
        template <class Callable, class ...Ts>
        class TaskFunc :public Task {
            using ReturnType =
                std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>;
            template<typename T>
            static void doAndSet(std::promise<T>& promise, Callable& callable, std::tuple<Ts...>& tuple) {
                promise.set_value(Apply(callable, tuple));
            }
            template<>
            static void doAndSet<void>(std::promise<void>& promise, Callable& callable, std::tuple<Ts...>& tuple) {
                Apply(callable, tuple);
                promise.set_value();
            }
        public:
            TaskFunc(Callable call, Ts&&... args) :
                mCallable(std::forward<Callable>(call)), mTuple(std::forward_as_tuple(args...)) {}
            void execute() override {
                try {
                    doAndSet<ReturnType>(mPromise, mCallable, mTuple);
                }
                catch (...) {
                    try {
                        mPromise.set_exception(std::current_exception());
                    }
                    catch (...) {}
                }
                delete this;
            }
            auto getFuture() { return mPromise.get_future(); }
        private:
            Callable mCallable;
            std::tuple<Ts...> mTuple;
            std::promise<ReturnType> mPromise;
        };

        template<typename Range, typename Callable>
        class SubTask :public Task {
        public:
            SubTask(const Callable& call, Range range, TaskGroupFuture& future, size_t atomic)
                :mCallable(call), mRange(range), mFuture(future), mAtomic(atomic), mLast(range.size() / atomic + static_cast<bool>(range.size() % atomic)) {}
            bool reusable() override {
                --mLast;
                return mLast;
            }
            void execute() override {
                size_t last;
                auto task = mRange.cut(mAtomic);
                try {
                    mCallable(task);
                    last = mFuture.finish(task.size());
                }
                catch (...) {
                    try {
                        last = mFuture.setException(std::current_exception(), task.size());
                    }
                    catch (...) {}
                }
                if (!last)delete this;
            }
        private:
            Range mRange;
            size_t mAtomic;
            size_t mLast;
            TaskGroupFuture& mFuture;
            Callable mCallable;
        };

    }

    namespace TaskGroupHelper {
        class CARBON_API IntegerRange final {
        private:
            std::atomic_size_t mBegin;
            const size_t mEnd;
        public:
            IntegerRange(size_t begin, size_t end);
            IntegerRange(const IntegerRange& rhs);
            IntegerRange cut(size_t atomic);
            size_t size() const;
            static auto forEach(const std::function<void(size_t)>& callable) {
                return [=](IntegerRange range) {
                    size_t begin = range.mBegin.load();
                    while (begin < range.mEnd) { callable(begin); ++begin; }
                };
            }
        };
    }

    template<class Callable, class ...Ts>
    inline auto Async(ThreadPool& pool, const Callable& callable, Ts&&... args) {
        auto newTask = new TppDetail::TaskFunc<Callable, Ts...>(callable, std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        pool.addTask(newTask);
        return fut;
    }

    template<typename Range, typename Callable>
    inline auto AsyncGroup(ThreadPool& pool, Range range, const Callable& closure, size_t atomic = 0) {
        auto fut = std::make_unique<TaskGroupFuture>(range.size());
        if (atomic == 0)atomic = range.size() / pool.size() + 1;
        auto newTask = new TppDetail::SubTask<Range, Callable>(closure, range, *fut, atomic);
        pool.addTask(newTask);
        return std::move(fut);
    }

}
