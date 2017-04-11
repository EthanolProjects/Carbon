#include "ThreadPool.hpp"
#include <algorithm>

namespace Carbon {
    namespace TppDetail {
        TaskQueue::TaskQueue() {}
        TaskQueue::~TaskQueue() {}

        void TaskQueue::addTask(Task* task) {
            //std::lock_guard<std::mutex> lk(mMutex);
            mQueue.push(task);
        }

        Task* TaskQueue::getTask() {
            //std::lock_guard<std::mutex> lk(mMutex);
            Task* ret = nullptr;
            if (mQueue.size()) {
                //auto task = std::move(mQueue.front());
                mQueue.pop(ret);
                //return task;
            }
            return ret;
        }

        void PoolThread::setSource(TaskQueue& source) {
            m_source = &source;
            m_thread = std::thread([this]() { runThread(); });
        }

        void PoolThread::runThread() {
            Task* task = nullptr;
            while (true) {
                while (m_source && !(task = m_source->getTask()))
                    std::this_thread::yield();
                if (!m_source)break;
                task->run();
                delete task;
            }
        }
        PoolThread::~PoolThread() {
            m_source = nullptr;
            if (m_thread.joinable())
                m_thread.join();
        }
    }
   
    ThreadPool::ThreadPool(size_t num) {
        if (num == 0)
            num = std::thread::hardware_concurrency();
        m_threads = std::make_unique<TppDetail::PoolThread[]>(num);
        for (size_t i = 0; i < num; ++i)
            m_threads[i].setSource(m_source);
    }
    ThreadPool::~ThreadPool() {
        m_threads.reset();
    }
}