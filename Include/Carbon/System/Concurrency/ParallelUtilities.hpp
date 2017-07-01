#pragma once
#include "Threadpool.hpp"
#include "Carbon/MetaProgramming/Template/Mpl.hpp"
#include <memory>

#pragma warning (push)
#pragma warning (disable: 4250)

namespace Carbon {
    namespace {
        template<class T, class U, class Callable, class ...Ts>
        struct ApplyToImpl {
            static void doAndSet(U& promise, Callable& callable, std::tuple<Ts...>& tuple) {
                promise.set(Apply(callable, tuple));
            }
        };

        template<class U, class Callable, class ...Ts>
        struct ApplyToImpl<void, U, Callable, Ts...> {
            static void doAndSet(U& promise, Callable& callable, std::tuple<Ts...>& tuple) {
                Apply(callable, tuple);
                promise.set();
            }
        };

        template <class Callable, class ...Ts>
        class APCSingle : public AssocTpWorkStat<std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>> {
            using ReturnType = std::result_of_t<std::decay_t<Callable>(std::decay_t<Ts>...)>;
        public:
            APCSingle(const Callable& call, Ts&&... args) :
                mCallable(call), mTuple(std::forward_as_tuple(args...)) {}
            auto getFuture() { return Future<ReturnType>(this); }
        private:
            bool xRunWork() override {
                ApplyToImpl<ReturnType, decltype(*this), Callable, Ts...>::doAndSet(*this, mCallable, mTuple);
                return true;
            }
            Callable mCallable;
            std::tuple<Ts...> mTuple;
        };

        template <class Callable, class ForwIter>
        class APCForEach : public AssocTpWorkStat<void> {
        public:
            APCForEach(ForwIter begin, ForwIter end, const Callable& call):
                mBegin(begin), mEnd(end), mCallable(call) {}
            auto getFuture() { return Future<void>(this); }
        private:
            bool xRunWork() override {
                auto thisIter = mBegin++;
                if (thisIter != mEnd) {
                    submit();
                    mCallable(thisIter);
                    return false;
                }
                return true;
            }
            ForwIter mBegin, mEnd;
            Callable mCallable;
        };

    }

    template<class Callable, class ForwIter>
    inline auto asyncForRange(ForwIter begin, ForwIter end, const Callable& callable) {
        auto newTask = new APCForEach<Callable, ForwIter>(begin, end, callable);
        auto fut = newTask->getFuture();
        newTask->submit();
        return fut;
    }

    template<class Callable, class ...Ts>
    inline auto async(const Callable& callable, Ts&&... args) {
        auto newTask = new APCSingle<Callable, Ts...>(callable, std::forward<Ts>(args)...);
        auto fut = newTask->getFuture();
        newTask->submit();
        return fut;
    }
}
#pragma warning (pop)
