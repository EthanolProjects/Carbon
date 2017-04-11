#include <thread>
#include <atomic>
#include <memory>
#include <chrono>
#include <vector>
#include "Task.hpp"

namespace Carbon {
    class CARBON_API ThreadPool;
    namespace TppDetail {
        class CARBON_API LockFreeList;
        class CARBON_API PoolThread final {
        public:
            void setSource(LockFreeList& source);
            ~PoolThread();
        private:
            void runThread();
            std::thread m_thread;
            LockFreeList* m_source;
            bool m_flag;
        };

        class CARBON_API LockFreeList final {
        public:
            LockFreeList();
            ~LockFreeList();
            void addTask(std::unique_ptr<Task>&& task);
            std::unique_ptr<Task> getTask();
        private:
            struct Node final {
                std::unique_ptr<Task> task = nullptr;
                Node* next = nullptr;
            };
            std::atomic<Node*> m_head;
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
        TppDetail::LockFreeList m_source;
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
