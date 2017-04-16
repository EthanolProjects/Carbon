#include "Concurrency.hpp"
#include "../ThirdParty/LockFree/Queue.hpp"
#include <queue>

namespace Carbon {
    bool Task::last() const noexcept {
        return false;
    }
    std::function<void()> Task::cut() noexcept { return {}; }

    class ThreadPool::TaskQueue {
    public:
        using Queue_t = ArrayLockFreeQueue<Task*>;
        void addTask(Task* task) {
            while (true)
                if (mBack->push(task))return;
                else {
                    std::lock_guard<std::mutex> lock(mMutex);
                    if (mBack->full()) {
                        mQueue.push(std::make_unique<Queue_t>());
                        mBack = mQueue.back().get();
                    }
                }
        }
        Task* getTask() {
            Task* ret = nullptr;
            auto success = mFront->pop(ret);
            if (!success && mFront != mBack) {
                mMutex.lock();
                if (mQueue.size()>1) {
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
        TaskQueue() {
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
            m_thread = std::thread([this] () { runThread(); });
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
            if (task->isSingle) {
                task->run();
                delete task;
            }
            else {
                auto run = task->cut();
                if (task->last())
                    m_source->addTask(task);
                else delete task;
                run();
            }
        }
    }

    ThreadPool::PoolThread::~PoolThread() {
        m_flag = false;

        if (m_thread.joinable())
            m_thread.join();
    }

    ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

    ThreadPool::ThreadPool(size_t num):mSize(num) {
        mSource = std::make_unique<TaskQueue>();
        mThreads = std::make_unique<PoolThread[]>(num);
        for (size_t i = 0; i < num; ++i)
            mThreads[i].setSource(mSource.get());
    }

    void ThreadPool::addTask(Task* task) {
        mSource->addTask(task);
    }

    size_t ThreadPool::size() const {
        return mSize;
    }

    ThreadPool::~ThreadPool() {
        mThreads.reset();
    }

    TaskGroupFuture::TaskGroupFuture(size_t size) :mLast(size) {}
    TaskGroupFuture::~TaskGroupFuture(){
        wait();
    }
    void TaskGroupFuture::setException(std::exception_ptr exc,size_t size) {
        mExceptions.emplace_back(exc);
        mLast-=size;
    }

    void TaskGroupFuture::finish(size_t size) {
        mLast-=size;
    }

    void TaskGroupFuture::wait() const {
        while (mLast)std::this_thread::yield();
    }

    const std::vector<std::exception_ptr>& TaskGroupFuture::getExceptions() const {
        return mExceptions;
    }
    namespace TaskGroupHelper {
        IntegerRange::IntegerRange(size_t begin , size_t end) :mBegin(begin) , mEnd(end) {}
        IntegerRange IntegerRange::cut(size_t atomic) {
            auto lb = mBegin;
            mBegin += atomic;
            if (mBegin > mEnd)mBegin = mEnd;
            return { lb,mBegin };
        }
        size_t IntegerRange::size() const {
            return mEnd > mBegin ? mEnd - mBegin : 0;
        }
        std::function<void(IntegerRange)> IntegerRange::forEach
        (const std::function<void(size_t)>& callable) {
            return [=] (IntegerRange range) {
                while (range.mBegin < range.mEnd) { callable(range.mBegin); ++range.mBegin; }
            };
        }
    }
}
