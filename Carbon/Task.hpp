#include "Config.hpp"
#include "Mpl.hpp"
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
        virtual void run() = 0;
    };

    namespace TppDetail {
        // FIXME: Requires Better Implementation
        template <class Callable, class ...Ts>
        class TaskFunc : public Task {
        public:
            using ReturnType =
                std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>;
            TaskFunc(Callable call, Ts&&... args): 
                mCallable(call), mTuple(std::forward_as_tuple(args...)) 
            {}
            void run() override { Apply(mCallable , mTuple); }
        private:
            Callable mCallable;
            std::tuple<Ts...> mTuple;
        };

    }
    // FIXME: Requires Better Implementation
    template <class Callable>
    auto encloseTask(Callable callable) {
        return std::make_unique< TppDetail::TaskFunc<Callable>>(callable);
    }
}