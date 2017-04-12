#include <thread>
#include <memory>
#include <chrono>
#include <vector>
#include "Task.hpp"
#include "../ThirdParty/LockFree/lock_free_queue.h"

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
            void addTask(Task* task);
            Task* getTask();
        private:
            ArrayLockFreeQueue<Task*, 350000, 
                ArrayLockFreeQueueMultipleProducers> mQueue;
        };
    }

    class CARBON_API ThreadPool final {
    public:
        ThreadPool(size_t num = 0);
        void addTask(Task* task) {
            m_source.addTask(task);
        }
    private:
        std::unique_ptr<TppDetail::PoolThread[]> m_threads;
        TppDetail::TaskQueue m_source;
    };

    template<class Callable, class ...Ts>
    auto Async(ThreadPool& pool, Callable callable, Ts&&... args) {
        auto newTask = new TppDetail::TaskFunc<Callable, Ts...>(
            callable, std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        pool.addTask(newTask);
        return fut;
    }
}
