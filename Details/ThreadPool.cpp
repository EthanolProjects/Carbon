#include "Concurrency.hpp"
#include "PlatformControl.hpp"
#include "ThirdParty/LockFree/Queue.hpp"
#include <queue>
#include <condition_variable>

namespace Carbon {
#ifdef CARBON_TARGET_WINDOWS
    class GlobalThreadPool final : public ThreadPool {
    public:
        void addTask(Task* task) override {
            SubmitThreadpoolWork(CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE, PVOID taskIn, PTP_WORK work) {
                auto task = reinterpret_cast<Task*>(taskIn);
                if (task->reusable()) {
                    SubmitThreadpoolWork(work);
                    task->execute();
                }
                else {
                    task->execute();
                    CloseThreadpoolWork(work);
                }
            }, task, nullptr));
        }
    };
    class LocalThreadPool final : public ThreadPool {
    public:
        LocalThreadPool() {
            InitializeThreadpoolEnvironment(&mEnv);
        }
        ~LocalThreadPool() {
            DestroyThreadpoolEnvironment(&mEnv);
        }
        void addTask(Task* task) override {
            SubmitThreadpoolWork(CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE, PVOID taskIn, PTP_WORK work) {
                auto task = reinterpret_cast<Task*>(taskIn);
                if (task->reusable()) {
                    SubmitThreadpoolWork(work);
                    task->execute();
                }
                else {
                    task->execute();
                    CloseThreadpoolWork(work);
                }
            }, task, &mEnv));
        }
    private:
        TP_CALLBACK_ENVIRON mEnv;
    };

    ThreadPool& ThreadPool::getDefaultThreadPool() noexcept { static GlobalThreadPool pool; return pool; }
    std::unique_ptr<ThreadPool> ThreadPool::createThreadPool() { return std::make_unique<LocalThreadPool>(); }
#else
    class CARBON_API ThreadPool final {
    public:
        ThreadPool();
        ThreadPool(size_t num);
        ~ThreadPool();
        void addTask(Task* task);
        size_t size() const;
    private:
        class TaskQueue;
        class ThreadGroup;
        size_t mSize;
        std::unique_ptr<TaskQueue> mSource;
        std::unique_ptr<ThreadGroup> mThreads;
    };
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
                mThreads.emplace_back([this, source] () noexcept { runThread(source); });
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
                if (task->reusable())source->addTask(task);
                task->execute();
            }
        }
        std::mutex mSleep;
        std::atomic_bool mFlag;
        std::condition_variable mHolder;
        std::vector<std::thread> mThreads;
    };

    ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

    ThreadPool::ThreadPool(size_t num) : mSize(num) {
        mSource = std::make_unique<TaskQueue>();
        mThreads = std::make_unique<ThreadGroup>(mSource.get(), num);
    }

    void ThreadPool::addTask(Task* task) {
        mSource->addTask(task);
        mThreads->wakeAllOnDemand();
    }

    size_t ThreadPool::size() const {
        return mSize;
    }

    ThreadPool::~ThreadPool() {
        mThreads.reset();
    }
#endif

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
    void IntegerRange::forEach(const std::function<void(size_t)>& callable) const {
        size_t begin = mBegin.load();
        while (begin < mEnd) { callable(begin); ++begin; }
    }
    void IntegerRange::forEach(const std::function<void(IntegerRange)>& callable) const {
        callable(*this);
    }
}
