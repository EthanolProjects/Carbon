#include "Concurrency.hpp"
#include "CarbonTargetWindows.hpp"

#ifdef CARBON_TARGET_WINDOWS
namespace CarbonWindows {
    using namespace Carbon;
    class TpWindowsBase : public Threadpool {
    public:
        TpWindowsBase() noexcept : TpWindowsBase(nullptr) {}
        TpWindowsBase(PTP_CALLBACK_ENVIRON pEnv) noexcept : mPEnv(pEnv) {}
        void submit(Work* task) override {
            SubmitThreadpoolWork(CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE, PVOID taskIn, PTP_WORK work) {
                auto task = reinterpret_cast<Work*>(taskIn);
                task->execute();
                CloseThreadpoolWork(work);
            }, task, mPEnv));
        }
    private:
        PTP_CALLBACK_ENVIRON mPEnv;
    };

    class TpWindowsCustom final : public TpWindowsBase {
    public:
        TpWindowsCustom() :TpWindowsBase(&mEnv) { InitializeThreadpoolEnvironment(&mEnv); }
        ~TpWindowsCustom() { DestroyThreadpoolEnvironment(&mEnv); }
    private:
        TP_CALLBACK_ENVIRON mEnv;
    };
}

namespace Carbon {
    using namespace CarbonWindows;
    Threadpool& Threadpool::getDefault() noexcept { static TpWindowsBase pool; return pool; }
    std::unique_ptr<Threadpool> Threadpool::create() { return std::make_unique<TpWindowsCustom>(); }
}
#endif
