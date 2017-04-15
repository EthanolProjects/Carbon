#include "Concurrency.hpp"
#include "../ThirdParty/LockFree/Queue.hpp"
#include <queue>

namespace Carbon {
    class ThreadPool::TaskQueue {
    public:
        using Queue_t = ArrayLockFreeQueue<Task* , LOCK_FREE_Q_DEFAULT_SIZE>;
        void addTask(Task* task) {
            while (true)
                if (mBack->push(task))return;
                else{
                    std::lock_guard<std::mutex> lock(mMutex);
                    if (mBack->full()){
                        mQueue.push(std::make_unique<Queue_t>());
                        mBack = mQueue.back().get();
                    }
                }
        }
        Task* getTask() {
            Task* ret = nullptr;
            auto success = mFront->pop(ret);
            if (!success && mFront!=mBack){
                mMutex.lock();
                if (mFront != mBack){
                    auto queue = std::move(mQueue.front());
                    mQueue.pop();
                    mFront = mQueue.front().get();
                    mMutex.unlock();
                    Task* ret;
                    while (queue->pop(ret))addTask(ret);
                }
                else mMutex.unlock();
            }
            return success ? ret : nullptr;
        }
        TaskQueue(){
            mQueue.push(std::make_unique<Queue_t>());
            mFront = mBack = mQueue.front().get();
        }
    private:
        std::queue<std::unique_ptr<Queue_t>> mQueue;
        Queue_t* mFront;
        Queue_t* mBack;
        std::mutex mMutex;
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
        mSource=std::make_unique<TaskQueue>();
        mThreads = std::make_unique<PoolThread[]>(num);
        for (size_t i = 0; i < num; ++i)
            mThreads[i].setSource(mSource.get());
    }

    void ThreadPool::addTask(Task* task) {
        mSource->addTask(task);
    }

    ThreadPool::~ThreadPool() {
        mThreads.reset();
    }

}