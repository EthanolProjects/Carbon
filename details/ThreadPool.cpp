#include "Concurrency.hpp"
#include "ThirdParty/LockFree/Queue.hpp"
#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>

namespace Carbon {
    class ThreadPool::TaskQueue {
    public:
        using Queue_t = ArrayLockFreeQueue<Task*>;
        void addTask(Task* task) {
            while (true)
                if (mBack->push(task)) return;
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

    class ThreadPool::ThreadGroup {
    public:
        ThreadGroup(TaskQueue* source, size_t count){
            mFlag = true;
            for (size_t i = 0; i < count; ++i)
                mThreads.emplace_back([this, source]() { runThread(source); });
        }
        ~ThreadGroup() {
            mFlag = false;
            for (auto&& thread : mThreads)
                if (thread.joinable())
                    thread.join();
        }
    private:
        static constexpr size_t maxSleep = 1000;
        void runThread(TaskQueue* source) {
            Task* task = nullptr;
            size_t sleep = 1;
            while (mFlag) {
                while (mFlag && !(task = source->getTask())) {
                    if (++sleep > maxSleep)sleep = maxSleep;
                    std::this_thread::sleep_for(std::chrono::microseconds(sleep));
                }
                if (!mFlag) break;
                sleep = 1;
                if (!task->last())
                    source->addTask(task);
                task->run();
            }
        }
        std::atomic_bool mFlag;
        std::vector<std::thread> mThreads;
    };

    ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

    ThreadPool::ThreadPool(size_t num):mSize(num) {
        mSource = std::make_unique<TaskQueue>();
        mThreads = std::make_unique<ThreadGroup>(mSource.get(), num);
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
