#pragma once
#include "Carbon/Config/Config.hpp"
#include <chrono>
#ifdef CARBON_TARGET_POSIX
#include <mutex>
#include <condition_variable>
#endif

namespace Carbon {
    namespace System {
        class CARBON_API Event {
        public:
            Event(bool initIsSignaled);
            Event(Event&&) = delete;
            Event(const Event&) = delete;
            Event& operator = (Event&&) = delete;
            Event& operator = (const Event&) = delete;
            ~Event();
            void set();
            void reset();
            void wait();
            template <class Rep, class Period>
            bool waitFor(const std::chrono::duration<Rep, Period>& rt) {
                return waitFor(std::chrono::duration_cast<std::chrono::milliseconds>(rt));
            }
            template<class Clock, class Duration>
            bool waitUntil(const std::chrono::time_point<Clock, Duration>& tt) {
                return waitFor(tt - Clock::now());
            }
            void* native() noexcept;
        private:
#if defined(CARBON_TARGET_WINDOWS)
            void* mEvent; // HANDLE mEvent;
#else
            bool mMode;
            std::mutex mMut;
            std::condition_variable mCondVar;
#endif
        };

        template <> CARBON_API bool Event::waitFor<std::chrono::milliseconds::rep, std::milli>(const std::chrono::milliseconds&);

    }
}
