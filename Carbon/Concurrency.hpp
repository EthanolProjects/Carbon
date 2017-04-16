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
        Task() = default;
        Task(Task&&) = default;
        Task& operator = (Task&&) = default;
        Task(const Task&) = delete;
        Task& operator = (const Task&) = delete;
        virtual ~Task() = default;
        virtual void run() noexcept = 0;
        virtual bool last() const noexcept;
        virtual std::function<void()> cut() noexcept;
        bool isSingle;
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
        class PoolThread;
        std::unique_ptr<PoolThread[]> mThreads;
        size_t mSize;
        std::unique_ptr<TaskQueue> mSource;
    };

    class CARBON_API TaskGroupFuture;
    class CARBON_API Range;

    namespace TppDetail {
        // TODO: FIXME -- Requires Better Implementation
        template <class Callable , class ...Ts>
        class TaskFunc : public Task {
        public:
            using ReturnType =
                std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>;
            TaskFunc(Callable call , Ts&&... args) :
                mCallable(std::forward<Callable>(call)) , mTuple(std::forward_as_tuple(args...)){
                isSingle = true;
            }
            void run() noexcept override {
                try {
                    mPromise.set_value(Apply(mCallable , mTuple));
                }
                catch (...) {
                    try {
                        mPromise.set_exception(std::current_exception());
                    }
                    catch (...) {}
                }
            }
            auto getFuture() { return mPromise.get_future(); }
        private:
            Callable mCallable;
            std::tuple<Ts...> mTuple;
            std::promise<ReturnType> mPromise;
        };

        template<typename Range>
        class SubTask final : public Task {
        public:
            SubTask(const std::function<void(Range)>& call , Range range ,
                TaskGroupFuture& future , size_t atomic)
                :mCallable(call) , mRange(range) , mFuture(future) , mAtomic(atomic) {
                isSingle = false;
            }
            void run() noexcept override {}
            bool last() const noexcept override {
                return mRange.size();
            }
            std::function<void()> cut() noexcept override {
                auto range = mRange.cut(mAtomic);
                auto callable = mCallable;
                auto future = &mFuture;
                return [=] {
                    try {
                        callable(range);
                        future->finish(range.size());
                    }
                    catch (...) {
                        try {
                            future->setException(std::current_exception() , range.size());
                        }
                        catch (...) {}
                    }};
            }
        private:
            std::function<void(Range)> mCallable;
            Range mRange;
            TaskGroupFuture& mFuture;
            size_t mAtomic;
        };

    }

    template<class Callable , class ...Ts>
    inline auto Async(ThreadPool& pool , Callable callable , Ts&&... args) {
        auto newTask = new TppDetail::TaskFunc<Callable , Ts...>(
            callable , std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        pool.addTask(newTask);
        return fut;
    }


    class CARBON_API TaskGroupFuture final {
    public:
        TaskGroupFuture(size_t size);
        ~TaskGroupFuture();
        void wait() const;
        template<class Clock , class Duration>
        bool wait_until(const std::chrono::time_point<Clock , Duration>& time) const {
            while (mLast) {
                std::this_thread::yield();
                if (std::chrono::system_clock::now() >= time)
                    return false;
            }
            return true;
        }
        template<class Rep , class Period>
        bool wait_for(const std::chrono::duration<Rep , Period>& time) const {
            return wait_until(std::chrono::system_clock::now() + time);
        }
        const std::vector<std::exception_ptr>& getExceptions() const;
    private:
        template<typename T>
        friend class TppDetail::SubTask;
        void finish(size_t size);
        void setException(std::exception_ptr exc , size_t size);
        std::atomic_size_t mLast;
        std::vector<std::exception_ptr> mExceptions;
    };

    template<typename Range>
    inline auto AsyncGroup(ThreadPool& pool ,
        const std::function<void(Range)>& closure , Range range ,
        size_t atomic = 0) {
        auto fut = std::make_unique<TaskGroupFuture>(range.size());
        if (atomic == 0)atomic = range.size() / pool.size()+1;
        auto newTask = new TppDetail::SubTask<Range>(closure , range , *fut , atomic);
        pool.addTask(newTask);
        return std::move(fut);
    }

}
