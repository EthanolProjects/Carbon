#include "Config.hpp"
#include "Mpl.hpp"
namespace Carbon {
    namespace TppDetail {
        class CARBON_API Task {
        public:
            virtual ~Task();
        private:
            struct Impl;
            Impl* mImpl;
            Task();
            Task(Task&&);
            Task& operator = (Task&&);
            Task(const Task&) = delete;
            Task& operator = (const Task&) = delete;
            virtual void run();
        };
        // FIXME: Requires Better Implementation
        template <class Callable, class ...Ts>
        class TaskFunc : public Task {
        public:
            using ReturnType = 
                std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>
            TaskFunc(Callable call, Ts&&... args): 
                mCallable(call), mTuple(std::forward_as_tuple(args)) 
            {}
        private:
            Callable mCallable;
            std::tuple<Ts...> mTuple;
            void run() override { Apply(mCalleble, mTuple); }
        };
    }
    // FIXME: Requires Better Implementation
    template <class Callable>
    auto encloseTask(Callable callable) {
        return new TppDetail::TaskFunc<Callable>(callable);
    }
}