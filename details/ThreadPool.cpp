#include "Concurrency.hpp"
#include "ThirdParty/LockFree/Queue.hpp"
#include <queue>

namespace Carbon {
    bool Task::reusable() {
        return false;
    }

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
                if (mQueue.size() > 1) {
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
        ThreadGroup(TaskQueue* source, size_t count) {
            mFlag = true;
            for (size_t i = 0; i < count; ++i)
                mThreads.emplace_back([this, source] () { runThread(source); });
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
                if (task->reusable())source->addTask(task);
                task->execute();
            }
        }
        std::atomic_bool mFlag;
        std::vector<std::thread> mThreads;
    };

    ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

    ThreadPool::ThreadPool(size_t num) : mSize(num) {
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
    TaskGroupFuture::~TaskGroupFuture() {
        wait();
    }
    size_t TaskGroupFuture::setException(std::exception_ptr exc, size_t size) {
        std::lock_guard<std::mutex> lock(mMutex);
        mExceptions.emplace_back(exc);
        return mLast -= size;
    }

    size_t TaskGroupFuture::finish(size_t size) {
        return mLast -= size;
    }

    void TaskGroupFuture::wait() const {
        size_t last = mLast, current;
        using clock = std::chrono::high_resolution_clock;
        auto time = clock::now();
        while (mLast) {
            current = mLast;
            if (last != current) {
                auto now = clock::now();
                auto wait = (now - time)*current / (last - current);
                std::this_thread::sleep_for(wait / 100);
                time = now;
                last = current;
            }
            else std::this_thread::yield();
        }
    }

    void TaskGroupFuture::catchExceptions(const std::function<void(std::function<void()>)>& catchFunc) {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto&& exc : mExceptions) {
            catchFunc([&exc] {std::rethrow_exception(std::move(exc)); });
        }
        mExceptions.clear();
    }
    namespace TaskGroupHelper {
        IntegerRange::IntegerRange(size_t begin, size_t end) :mBegin(begin), mEnd(end) {}
        IntegerRange::IntegerRange(const IntegerRange & rhs)
            : mBegin(rhs.mBegin.load()), mEnd(rhs.mEnd) {}
        IntegerRange IntegerRange::cut(size_t atomic) {
            size_t lb = mBegin.load();
            size_t end;
            do end = std::min(lb + atomic, mEnd);
            while (!mBegin.compare_exchange_weak(lb, end));
            return { lb,end };
        }
        size_t IntegerRange::size() const {
            return mEnd > mBegin ? mEnd - mBegin : 0;
        }
        std::function<void(IntegerRange)> IntegerRange::forEach
        (const std::function<void(size_t)>& callable) {
            return [=] (IntegerRange range) {
                size_t begin = range.mBegin.load();
                while (begin < range.mEnd) { callable(begin); ++begin; }
            };
        }
    }
}
