#include "ThreadPool.hpp"
#include <algorithm>

namespace Carbon {
    namespace TppDetail {
        TaskQueue::TaskQueue() {}
        TaskQueue::~TaskQueue() {
        }

        void TaskQueue::addTask(Task* task) {
            mQueue.push(task);
        }

        Task* TaskQueue::getTask() {
            Task* ret = nullptr;
            auto success = mQueue.pop(ret);
            return success ? ret : nullptr;
        }

        void PoolThread::setSource(TaskQueue& source) {
            m_source = &source;
            m_flag = true;
            m_thread = std::thread([this]() { runThread(); });
        }

        void PoolThread::runThread() {
            Task* task = nullptr;
            while (m_flag) {
                while (m_flag && m_source && !(task = m_source->getTask()))
                    std::this_thread::yield();
                if (!m_flag)break;
                task->run();
                delete task;
            }
        }

        PoolThread::~PoolThread() {
            m_flag = false;
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

}