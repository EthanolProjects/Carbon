#include "Concurrency.hpp"
#include "CarbonTargetWindows.hpp"

#ifdef CARBON_TARGET_WINDOWS
namespace CarbonWindows {
    using namespace Carbon;
    class TpWindows : public Threadpool {
    public:
        TpWindows() noexcept{
            mPool = CreateThreadpool(nullptr);
            InitializeThreadpoolEnvironment(&mEnv);
            SetThreadpoolCallbackPool(&mEnv, mPool);
            //SetThreadpoolCallbackCleanupGroup(&mEnv, mCleanup, nullptr);
        }
        ~TpWindows() {
            //CloseThreadpoolCleanupGroup(mCleanup);
            DestroyThreadpoolEnvironment(&mEnv);
            CloseThreadpool(mPool);
        }
        void submitOnce(Work* task) override {
            while(!TrySubmitThreadpoolCallback(simpleThreadpoolCallback, task, &mEnv));
        }
    private:
        PTP_POOL mPool;
        TP_CALLBACK_ENVIRON mEnv;
        PTP_CLEANUP_GROUP mCleanup;
        static void CALLBACK simpleThreadpoolCallback(PTP_CALLBACK_INSTANCE, PVOID taskIn) noexcept {
            reinterpret_cast<Work*>(taskIn)->main();
        }
    };
}

namespace Carbon {
    using namespace CarbonWindows;
    Threadpool& Threadpool::getDefault() noexcept { static TpWindows pool; return pool; }
    std::unique_ptr<Threadpool> Threadpool::create() { return std::make_unique<TpWindows>(); }
}
#endif
