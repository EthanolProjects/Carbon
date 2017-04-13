#include "Concurrency.hpp"
#include "../ThirdParty/LockFree/lock_free_queue.h"
#include <algorithm>

namespace Carbon {
    class ThreadPool::TaskQueue {
    public:
        void addTask(Task* task) {
            mQueue.push(task);
        }
        Task* getTask() {
            Task* ret = nullptr;
            auto success = mQueue.pop(ret);
            return success ? ret : nullptr;
        }
    private:
        ArrayLockFreeQueue<Task*, 350000> mQueue;
    };

    class ThreadPool::PoolThread {
    public:
        ~PoolThread();
        void setSource(TaskQueue* source) {
        m_source = source;
        m_flag = true;
        m_thread = std::thread([this]() { runThread(); });
    }
    private:
        void runThread();
        bool m_flag;
        TaskQueue* m_source;
        std::thread m_thread;
    };

    void ThreadPool::PoolThread::runThread() {
        Task* task = nullptr;
        while (m_flag) {
            while (m_flag && m_source && !(task = m_source->getTask()))
                std::this_thread::yield();
            if (!m_flag)break;
            task->run();
            delete task;
        }
    }

    ThreadPool::PoolThread::~PoolThread() {
        m_flag = false;
        if (m_thread.joinable())
            m_thread.join();
    }

    ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

    ThreadPool::ThreadPool(size_t num) {
        m_source = std::make_unique<TaskQueue>();
        m_threads = std::make_unique<PoolThread[]>(num);
        for (size_t i = 0; i < num; ++i)
            m_threads[i].setSource(m_source.get());
    }

    void ThreadPool::addTask(Task* task) {
        m_source->addTask(task);
    }

    ThreadPool::~ThreadPool() {
        m_threads.reset();
        m_source.reset();
    }

}