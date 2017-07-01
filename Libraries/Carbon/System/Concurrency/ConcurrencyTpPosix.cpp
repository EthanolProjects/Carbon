#include "Carbon/System/Concurrency/Threadpool.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#ifdef CARBON_TARGET_POSIX
namespace CarbonPosix {
    using namespace Carbon;
    class TaskQueue {
    public:
        void push(AssocTpWorkStatShared* node) {
            std::unique_lock<std::mutex> lk(mMutex);
            mQueue.push(node);
        }

        bool pop(AssocTpWorkStatShared*& nodeOut) {
            std::unique_lock<std::mutex> lk(mMutex);
            if (mQueue.empty())
                return false;
            else {
                nodeOut = mQueue.front();
                mQueue.pop();
            }
            return true;
        }
    private:
        std::mutex mMutex;
        std::queue<AssocTpWorkStatShared*> mQueue;
    };

    class Threadpool final{
    public:
        Threadpool(int size) {
            for (int i = 0; i < size; ++i)
                mThreads.push_back(std::move(std::thread([this]() { worker(); })));
        }
        ~Threadpool() {
            mStop.store(true);
            wakeAllAndStopHolding();
            for (auto&& thread : mThreads)
                if (thread.joinable())
                    thread.join();
        }
        void worker() noexcept {
            while (!(mStop.load())) {
                AssocTpWorkStatShared* task;
                while (mQueue.pop(task)) {
                    task->runWork();
                    if (task->decCount() == 1)
                        delete task;
                }
                holdCurrenctThread();
            }
        }
        void addTask(AssocTpWorkStatShared* task) {
            task->incCount();
            mQueue.push(task);
            wakeOneThread();
        }
        static auto& getInstance() {
            static Threadpool pool(std::thread::hardware_concurrency());
            return pool;
        }
        void holdCurrenctThread() {
            std::unique_lock<std::mutex> lk(mHoldMutex);
            if (mIsHoldingAval)
                mHoldVariable.wait(lk);
        }
        void wakeOneThread() {
            std::unique_lock<std::mutex> lk(mHoldMutex);
            mHoldVariable.notify_one();
        }
        void wakeAllAndStopHolding() {
            std::unique_lock<std::mutex> lk(mHoldMutex);
            mIsHoldingAval = false;
            mHoldVariable.notify_all();
        }
    private:
        TaskQueue mQueue;
        int mWaitCount{ 0 };
        std::mutex mHoldMutex;
        bool mIsHoldingAval{ true };
        std::condition_variable mHoldVariable;
        volatile std::atomic_bool mStop{ false };
        std::vector<std::thread> mThreads;
    };
}

namespace Carbon {
    using namespace CarbonPosix;

    AssocTpWorkStatShared::AssocTpWorkStatShared() : mEvent(false), mExceptionPointer(nullptr) {}

    AssocTpWorkStatShared::~AssocTpWorkStatShared() {}

    void AssocTpWorkStatShared::xGet() {
        wait();
        if (mExceptionPointer != nullptr)
            std::rethrow_exception(mExceptionPointer);
    }

    bool AssocTpWorkStatShared::xRunWork() {
        return true;
    }

    void AssocTpWorkStatShared::wait() { mEvent.wait(); }

    bool AssocTpWorkStatShared::xWaitFor(long long milli) {
        return mEvent.waitFor(std::chrono::milliseconds(milli));
    }

    bool AssocTpWorkStatShared::runWork() {
        try {
            if (this->xRunWork()) {
                mEvent.set();
                return true;
            }
            else
                return false;
        }
        catch (...) {
            mExceptionPointer = std::current_exception();
            mEvent.set();
        }
        return true;
    }

    void AssocTpWorkStatShared::submit() { Threadpool::getInstance().addTask(this); };
}
#endif
