#pragma once
#include <thread>
namespace Carbon {
    namespace Concurrency {
        inline auto hardwareConcurrency() noexcept{ return std::thread::hardware_concurrency(); }

        namespace Details {
            int getBusyCount() noexcept;
            void increaseBusyCount() noexcept;
            void decreaseBusyCount() noexcept;
            bool overSubscript() noexcept { getBusyCount() > hardwareConcurrency(); }
        }

        namespace ThisThread {
            inline auto getID() noexcept { return std::this_thread::get_id(); }

            inline void yield() noexcept {
                Details::decreaseBusyCount();
                std::this_thread::yield();
                Details::increaseBusyCount();
            }

            template<class Clock, class Duration>
            inline void sleepUntil(const std::chrono::time_point<Clock, Duration>& sleep) {
                Details::decreaseBusyCount();
                std::this_thread.sleep_until(sleep);
                Details::increaseBusyCount();
            }

            template<class Rep, class Period>
            inline void sleepFor(const std::chrono::duration<Rep, Period>& sleep) {
                Details::decreaseBusyCount();
                std::this_thread.sleep_for(sleep);
                Details::increaseBusyCount();
            }
        }

    }
}
