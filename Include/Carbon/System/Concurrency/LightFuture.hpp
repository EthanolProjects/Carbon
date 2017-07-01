#pragma once
#include "Carbon/Config/Config.hpp"
#include <chrono>
#include <atomic>
namespace Carbon {
    class CARBON_API AtomicCounter {
    public:
        virtual ~AtomicCounter() {}
        int incCount() noexcept { return x.fetch_add(1); }
        int decCount() noexcept { return x.fetch_sub(1); }
    private:
        std::atomic_int x{ 0 };
    };

    class CARBON_API AssocStateBase : public AtomicCounter {
    public:
        virtual void wait() = 0;
        template <class Rep, class Period>
        bool waitFor(const std::chrono::duration<Rep, Period>& rt) {
            return xWaitFor(std::chrono::duration_cast<std::chrono::milliseconds>(rt).count());
        }
        template<class Clock, class Duration>
        bool waitUntil(const std::chrono::time_point<Clock, Duration>& tt) {
            return waitFor(tt - Clock::now());
        }
    protected:
        virtual bool xWaitFor(long long milli) = 0;
    };

    template <class T>
    class AssocState : virtual public AssocStateBase {
    public:
        virtual T get() = 0;
        virtual void set(const T&) = 0;
    };

    template <>
    class AssocState<void> : virtual public AssocStateBase {
    public:
        virtual void set() = 0;
        virtual void get() = 0;
    };

    template <class T>
    class Future {
    public:
        Future(AssocState<T>* r) { r->incCount(); x = r; }
        Future(const Future&) = delete;
        Future& operator = (const Future&) = delete;
        Future(Future&& r) : x(r.x) { r.x = nullptr; }
        Future& operator = (Future&& r) { x = r.x, r.x = nullptr; }
        ~Future() { if (x) if (x->decCount() == 1) delete x; }
        auto get() { return x->get(); }
        void wait() { x->wait(); }
        template <class Rep, class Period>
        bool waitFor(const std::chrono::duration<Rep, Period>& rt) { return x->waitFor(rt); }
        template<class Clock, class Duration>
        bool waitUntil(const std::chrono::time_point<Clock, Duration>& tt) { return x->waitUntil(tt); }
    private:
        AssocState<T>* x;
    };

    template <class T>
    class Promise {
    public:
        Promise(AssocState<T>* r) { r->incCount(); x = r; }
        Promise(const Promise&) = delete;
        Promise& operator = (const Promise&) = delete;
        Promise(Promise&& r) : x(r.x) { r.x = nullptr; }
        Promise& operator = (Promise&& r) { x = r.x, r.x = nullptr; }
        ~Promise() { if (x) if (x->decCount() == 1) delete x; }
        void set(const T& v) { x->set(v); }
        auto getFuture() { return Future<T>(x); }
    private:
        AssocState<T>* x;
    };

    template <>
    class Future<void> {
    public:
        Future(AssocState<void>* r) { r->incCount(); x = r; }
        Future(const Future&) = delete;
        Future& operator = (const Future&) = delete;
        Future(Future&& r) : x(r.x) { r.x = nullptr; }
        Future& operator = (Future&& r) { x = r.x, r.x = nullptr; }
        ~Future() { if (x) if (x->decCount() == 1) delete x; }
        void get() { x->get(); }
        void wait() { x->wait(); }
        template <class Rep, class Period>
        bool waitFor(const std::chrono::duration<Rep, Period>& rt) { return x->waitFor(rt); }
        template<class Clock, class Duration>
        bool waitUntil(const std::chrono::time_point<Clock, Duration>& tt) { return x->waitUntil(tt); }
    private:
        AssocState<void>* x;
    };

    template <>
    class Promise<void> {
    public:
        Promise(AssocState<void>* r) { r->incCount(); x = r; }
        Promise(const Promise&) = delete;
        Promise& operator = (const Promise&) = delete;
        Promise(Promise&& r) : x(r.x) { r.x = nullptr; }
        Promise& operator = (Promise&& r) { x = r.x, r.x = nullptr; }
        ~Promise() { if (x) if (x->decCount() == 1) delete x; }
        void set() { x->set(); }
        auto getFuture() { return Future<void>(x); }
    private:
        AssocState<void>* x;
    };

}
