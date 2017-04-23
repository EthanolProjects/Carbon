#pragma once
#include "Config.hpp"
#include "Mpl.hpp"
#include <thread>
#include <memory>
#include <vector>
#include <future>

namespace Carbon {
    struct CARBON_API Task {
        virtual void execute();
        virtual bool reusable(std::function<void()>&, bool&);
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
        template<typename T> struct SubTask;
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
        void TaskGroupFuture::catchExceptions(const std::function<void(std::function<void()>)>& catchFunc);
    private:
        template<typename T>
        friend class TppDetail::SubTask;
        size_t finish(size_t size);
        size_t setException(std::exception_ptr exc, size_t size);
        std::atomic_size_t mLast;
        std::vector<std::exception_ptr> mExceptions;
        std::mutex mMutex;
    };

    namespace TppDetail {
        // TODO: FIXME -- Requires Better Implementation
        template <class Callable, class ...Ts>
        struct TaskFunc : Task {
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
            TaskFunc(Callable call, Ts&&... args) :
                callable(std::forward<Callable>(call)), tuple(std::forward_as_tuple(args...)) {}
            void execute() override {
                try {
                    doAndSet<ReturnType>(promise, callable, tuple);
                }
                catch (...) {
                    try {
                        promise.set_exception(std::current_exception());
                    }
                    catch (...) {}
                }
                delete this;
            }
            auto getFuture() { return promise.get_future(); }
            Callable callable;
            std::tuple<Ts...> tuple;
            std::promise<ReturnType> promise;
        };

        template<typename Range>
        struct SubTask : Task {
            SubTask(const std::function<void(Range)>& call, Range range,
                TaskGroupFuture& future, size_t atomic)
                :callable(call), range(range), future(future), atomic(atomic) {}
            bool reusable(std::function<void()>& func, bool& reuse)override {
                Range task = range.cut(atomic);
                func = [this, task] {
                    size_t last;
                    try {
                        callable(task);
                        last = future.finish(task.size());
                    }
                    catch (...) {
                        try {
                            last = future.setException(std::current_exception(), task.size());
                        }
                        catch (...) {}
                    }
                    if (!last)delete this;
                };
                reuse = static_cast<bool>(range.size());
                return true;
            }
            Range range;
            size_t atomic;
            TaskGroupFuture& future;
            std::function<void(Range)> callable;
        };

    }

    namespace TaskGroupHelper {
        class CARBON_API IntegerRange final {
        private:
            size_t mBegin, mEnd;
        public:
            IntegerRange(size_t begin, size_t end);
            IntegerRange cut(size_t atomic);
            size_t size() const;
            static std::function<void(IntegerRange)> forEach(const std::function<void(size_t)>& callable);
        };
    }

    template<class Callable, class ...Ts>
    inline auto Async(ThreadPool& pool, Callable callable, Ts&&... args) {
        auto newTask = new TppDetail::TaskFunc<Callable, Ts...>(
            callable, std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        pool.addTask(newTask);
        return fut;
    }

    template<typename Range>
    inline auto AsyncGroup(ThreadPool& pool,
        const std::function<void(Range)>& closure, Range range,
        size_t atomic = 0) {
        auto fut = std::make_unique<TaskGroupFuture>(range.size());
        if (atomic == 0)atomic = range.size() / pool.size() + 1;
        auto newTask = new TppDetail::SubTask<Range>(closure, range, *fut, atomic);
        pool.addTask(newTask);
        return std::move(fut);
    }

}
