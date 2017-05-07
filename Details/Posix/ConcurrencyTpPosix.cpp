#include "Concurrency.hpp"
#include "../ThirdParty/LockFree/Queue.hpp"
#include <queue>
#include <condition_variable>

#ifdef CARBON_TARGET_POSIX
namespace CarbonPosix {
    class CARBON_API Threadpool final {
    public:
        Threadpool();
        Threadpool(size_t num);
        ~Threadpool();
        void submit(Task* task);
        size_t size() const;
    private:
        class TaskQueue;
        class ThreadGroup;
        size_t mSize;
        std::unique_ptr<TaskQueue> mSource;
        std::unique_ptr<ThreadGroup> mThreads;
    };
    class Threadpool::TaskQueue {
    public:
        using Queue_t = ArrayLockFreeQueue<Task*>;
        void submit(Task* task) {
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
                if (mQueue.size() > 1) {
                    auto queue = std::move(mQueue.front());
                    mQueue.pop();
                    mFront = mQueue.front().get();
                    mMutex.unlock();
                    Task* ret;
                    while (queue->pop(ret))submit(ret);
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

    class Threadpool::ThreadGroup {
    public:
        ThreadGroup(TaskQueue* source, size_t count) {
            mFlag = true;
            for (size_t i = 0; i < count; ++i)
                mThreads.emplace_back([this, source]() noexcept { runThread(source); });
        }
        ~ThreadGroup() {
            mFlag = false;
            wakeAllOnDemand();
            for (auto&& thread : mThreads)
                if (thread.joinable())
                    thread.join();
        }
        void wakeOneOnDemand() noexcept { mHolder.notify_one(); }
        void wakeAllOnDemand() noexcept { mHolder.notify_all(); }
    private:
        void runThread(TaskQueue* source) noexcept {
            Task* task = nullptr;
            size_t sleep = 1;
            while (mFlag) {
                while (mFlag && !(task = source->getTask())) {
                    // TODO: apply a rotation lock is necessary
                    std::unique_lock<std::mutex> lock(mSleep);
                    mHolder.wait(lock); // Hold the thread
                }
                if (!mFlag) break;
                if (task->reusable())source->submit(task);
                task->execute();
            }
        }
        std::mutex mSleep;
        std::atomic_bool mFlag;
        std::condition_variable mHolder;
        std::vector<std::thread> mThreads;
    };

    Threadpool::Threadpool() : Threadpool(std::thread::hardware_concurrency()) {}

    Threadpool::Threadpool(size_t num) : mSize(num) {
        mSource = std::make_unique<TaskQueue>();
        mThreads = std::make_unique<ThreadGroup>(mSource.get(), num);
    }

    void Threadpool::submit(Task* task) {
        mSource->submit(task);
        mThreads->wakeAllOnDemand();
    }

    size_t Threadpool::size() const {
        return mSize;
    }

    Threadpool::~Threadpool() {
        mThreads.reset();
    }

}
#endif
