#include "Carbon/System/Concurrency/Threadpool.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#ifdef CARBON_TARGET_POSIX
namespace CarbonPosix {
    using namespace Carbon;

    class Threadpool final{
    public:
        Threadpool(int size) {
            for (int i = 0; i < size; ++i)
                mThreads.push_back(std::move(std::thread([this]() { worker(); })));
        }
        ~Threadpool() {
            std::unique_lock<std::mutex> lk(mMutex);
            mRun = false;
            mHoldVariable.notify_all();
            for (auto&& thread : mThreads) if (thread.joinable()) thread.join();
        }
        void worker() noexcept {
            AssocTpWorkStatShared* task = nullptr;
            std::unique_lock<std::mutex> lk(mMutex);
            for (;;) {
                if (mQueue.size()) {
                    task = mQueue.front();
                    mQueue.pop();
                    lk.unlock();
                    task->runWork();
                    if (task->decCount() == 1) delete task;
                    lk.lock();
                }
                else 
                    if (mRun) mHoldVariable.wait(lk); else return;
            }
        }
        void addTask(AssocTpWorkStatShared* task) {
            std::unique_lock<std::mutex> lk(mMutex);
            task->incCount();
            mQueue.push(task);
            mHoldVariable.notify_one();
        }
        static auto& getInstance() {
            static Threadpool pool(std::thread::hardware_concurrency());
            return pool;
        }
    private:
        std::queue<AssocTpWorkStatShared*> mQueue;
        bool mRun{ true };
        std::mutex mMutex;
        std::condition_variable mHoldVariable;
        std::vector<std::thread> mThreads;
    };
}

namespace Carbon {
    using namespace CarbonPosix;

    AssocTpWorkStatShared::AssocTpWorkStatShared() {}

    AssocTpWorkStatShared::~AssocTpWorkStatShared() {}

    bool AssocTpWorkStatShared::xRunWork() {
        return true;
    }

    bool AssocTpWorkStatShared::runWork() {
        try {
            return this->xRunWork();
        }
        catch (...) {
            setException(std::current_exception());
        }
        return true;
    }

    void AssocTpWorkStatShared::submit() { Threadpool::getInstance().addTask(this); };
}
#endif
