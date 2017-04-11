#include "Config.hpp"
#include "Mpl.hpp"
#include <future>
namespace Carbon {
    class CARBON_API Task
    {
    public:
        Task() = default;
        Task(Task&&) = default;
        Task& operator = (Task&&) = default;
        Task(const Task&) = delete;
        Task& operator = (const Task&) = delete;
        virtual ~Task() = default;
        virtual void run() noexcept = 0;
    };

    namespace TppDetail {
        // FIXME: Requires Better Implementation
        template <class Callable, class ...Ts>
        class TaskFunc : public Task {
        public:
            using ReturnType =
                std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>;
            TaskFunc(Callable call, Ts&&... args): 
                mCallable(std::forward<Callable>(call)), mTuple(std::forward_as_tuple(args...)) 
            {}
            void run() noexcept override { 
                try {
                    mPromise.set_value(Apply(mCallable, mTuple));
                }
                catch (...) {
                    try {
                        mPromise.set_exception(std::current_exception());
                    }
                    catch (...) {} 
                }
            }
            auto getFuture() { return mPromise.get_future(); }
        private:
            Callable mCallable;
            std::tuple<Ts...> mTuple;
            std::promise<ReturnType> mPromise;
        };

    }
}