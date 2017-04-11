#include <thread>
#include <atomic>
#include <memory>
#include <chrono>
#include <mutex>
#include <queue>
#include <vector>
#include "Task.hpp"

namespace Carbon {
    class CARBON_API ThreadPool;
    namespace TppDetail {
        class CARBON_API TaskQueue;
        class CARBON_API PoolThread final {
        public:
            void setSource(TaskQueue& source);
            ~PoolThread();
        private:
            void runThread();
            std::thread m_thread;
            TaskQueue* m_source;
            bool m_flag;
        };

        class CARBON_API TaskQueue final {
        public:
            TaskQueue();
            ~TaskQueue();
            void addTask(std::unique_ptr<Task>&& task);
            std::unique_ptr<Task> getTask();
        private:
            std::mutex mMutex;
            std::queue<std::unique_ptr<Task>> mQueue;
        };
    }

    class CARBON_API ThreadPool final {
    public:
        ThreadPool(size_t num = 0);
        void addTask(std::unique_ptr<Task>&& task) {
            m_source.addTask(std::move(task));
        }
    private:
        std::unique_ptr<TppDetail::PoolThread[]> m_threads;
        TppDetail::TaskQueue m_source;
    };

    template<class Callable, class ...Ts>
    auto Async(ThreadPool& pool, Callable callable, Ts&&... args) {
        auto newTask = std::make_unique<TppDetail::TaskFunc<Callable, Ts...>>(
            callable, std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        pool.addTask(std::move(newTask));
        return fut;
    }
}
