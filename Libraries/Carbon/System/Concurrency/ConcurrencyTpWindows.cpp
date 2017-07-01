#include "Carbon/System/Concurrency/Threadpool.hpp"
#include "../../Targets/CarbonTargetWindows.hpp"

#ifdef CARBON_TARGET_WINDOWS

namespace Carbon {
    static void CALLBACK __TpWin32Callback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK) noexcept {
        auto work = reinterpret_cast<AssocTpWorkStatShared*>(Context);
        work->runWork();
        if (work->decCount() == 1)
            delete work;
    }

    AssocTpWorkStatShared::AssocTpWorkStatShared():
        mEvent(false), mExceptionPointer(nullptr) {
        mWorkHandle = CreateThreadpoolWork(__TpWin32Callback, this, nullptr);
    }

    AssocTpWorkStatShared::~AssocTpWorkStatShared() {
        CloseThreadpoolWork(reinterpret_cast<PTP_WORK>(mWorkHandle));
    }

    void AssocTpWorkStatShared::xGet() {
        wait();
        if (mExceptionPointer != nullptr)
            std::rethrow_exception(mExceptionPointer);
    }

    bool AssocTpWorkStatShared::xRunWork() {
        return true;
    }

    void AssocTpWorkStatShared::wait() {
        WaitForThreadpoolWorkCallbacks(reinterpret_cast<PTP_WORK>(mWorkHandle), false);
    }

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

    void AssocTpWorkStatShared::submit() {
        incCount();
        SubmitThreadpoolWork(reinterpret_cast<PTP_WORK>(mWorkHandle));
    }

}
#endif
