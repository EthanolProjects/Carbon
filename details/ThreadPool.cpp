#include "ThreadPool.hpp"
#include <algorithm>

namespace Carbon {
    namespace TppDetail {
        TaskQueue::TaskQueue() {}
        TaskQueue::~TaskQueue() {
        }

        void TaskQueue::addTask(std::unique_ptr<Task>&& task) {
            std::lock_guard<std::mutex> lk(mMutex);
            mQueue.push(std::move(task));
        }

        std::unique_ptr<Task> TaskQueue::getTask() {
            std::lock_guard<std::mutex> lk(mMutex);
            if (mQueue.size()) {
                auto task = std::move(mQueue.front());
                mQueue.pop();
                return task;
            }
            return nullptr;
        }

        void PoolThread::setSource(TaskQueue& source) {
            m_source = &source;
            m_flag = true;
            m_thread = std::thread([this]() { runThread(); });
        }

        void PoolThread::runThread() {
            std::unique_ptr<Task> task;
            while (m_flag) {
                while (m_flag && !(task = m_source->getTask()))
                    std::this_thread::yield();
                if (!m_flag)break;
                task->run();
                task.reset();
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