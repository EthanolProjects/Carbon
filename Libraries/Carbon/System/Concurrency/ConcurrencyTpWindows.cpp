#include "Carbon/System/Concurrency/Threadpool.hpp"
#include "../../Targets/CarbonTargetWindows.hpp"

#ifdef CARBON_TARGET_WINDOWS

namespace Carbon {
    namespace {
        static void CALLBACK TpWin32Callback(PTP_CALLBACK_INSTANCE Instance, PVOID Context) noexcept {
            auto work = reinterpret_cast<AssocTpWorkStatShared*>(Context);
            work->runWork();
            if (work->decCount() == 1)
                delete work;
        }
    }

    AssocTpWorkStatShared::AssocTpWorkStatShared() {}

    AssocTpWorkStatShared::~AssocTpWorkStatShared() {}

    bool AssocTpWorkStatShared::xRunWork() {
        return true;
    }

    bool AssocTpWorkStatShared::runWork() {
        try {
            if (this->xRunWork()) {
                return true;
            }
            else
                return false;
        }
        catch (...) {
            catchCurrentException();
        }
        return true;
    }

    void AssocTpWorkStatShared::submit() {
        incCount();
        while(!TrySubmitThreadpoolCallback(&TpWin32Callback, this, nullptr));
    }

}
#endif
