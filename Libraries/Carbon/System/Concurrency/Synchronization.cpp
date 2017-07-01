#include "Carbon/System/Concurrency/Synchronization.hpp"
#include "../../Targets/CarbonTargetWindows.hpp"

namespace Carbon {
    namespace System {
#if defined (CARBON_TARGET_WINDOWS)
        Event::Event(bool initIsSignaled) {
            mEvent = CreateEvent(nullptr, true, initIsSignaled, nullptr);
        }

        Event::~Event() {}

        void Event::set() {
            SetEvent(mEvent);
        }

        void Event::reset() {
            ResetEvent(mEvent);
        }

        void Event::wait() {
            WaitForSingleObject(mEvent, INFINITE);
        }

        void * Event::native() noexcept {
            return mEvent;
        }

        bool Event::xWaitFor(unsigned long long milliseconds) {
            auto stat = WaitForSingleObject(mEvent, milliseconds);
            return stat == WAIT_OBJECT_0;
        }

#else
        Event::Event(bool initIsSignaled): mMode(initIsSignaled) {}

        Event::~Event() {}

        void Event::set() {
            std::unique_lock<std::mutex> lk(mMut);
            mMode = true;
            mCondVar.notify_all();
        }

        void Event::reset() {
            std::unique_lock<std::mutex> lk(mMut);
            mMode = false;
        }

        void Event::wait() {
            std::unique_lock<std::mutex> lk(mMut);
            if (!mMode)
                mCondVar.wait(lk);
        }

        void* Event::native() noexcept {
            return this;
        }

        bool Event::xWaitFor(unsigned long long milliseconds) {
            std::unique_lock<std::mutex> lk(mMut);
            if (!mMode)
                mCondVar.wait_for(lk, std::chrono::milliseconds(milliseconds));
        }
#endif
    }
}
