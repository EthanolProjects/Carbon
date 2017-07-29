#pragma once
#include "Carbon/System/Memory/ReferenceCounted.hpp"
#include "Synchronization.hpp"
#include <chrono>
#include <exception>
namespace Carbon {
    class CARBON_API PromiseException final : public std::exception {
    public:
        enum class Code :size_t {
            PromiseAlreadySatisfied = 0,
            PromiseBroken = 1,
            PromisedResultAlreadyRetrieved = 2
        };
        PromiseException(Code);
        ~PromiseException();
        PromiseException(const PromiseException&);
        PromiseException& operator = (const PromiseException&);
        Code errorCode() const noexcept;
        char const* what() const noexcept;
    private:
        Code mCode;
    };

    class CARBON_API PromiseImplBase : public ReferenceCounted {
    public:
        void wait();
        void abandon();
        void catchCurrentException();
        bool isSatisfied() const noexcept;
        void setException(std::exception_ptr);
        template <class T>
        bool waitFor(const T& arg) { return mEvent.waitFor(arg); }
        template<class T>
        bool waitUntil(const T& arg) { return mEvent.waitUntil(arg); }
    protected:
        enum class Status : size_t { Waiting, ValueSet, ExceptionSet, Broken, Retrived };
        PromiseImplBase();
        ~PromiseImplBase();
        void getImpl();
        void throwIfSatisfied();
        void satisfy(Status) noexcept;
    private:
        std::atomic<Status> mStatus;
        System::Event mEvent;
        std::exception_ptr mExceptionPointer;
    };

    template <class T>
    class PromiseImpl : virtual public PromiseImplBase {
    public:
        T get() { getImpl(); return std::move(mStorage); }
        void setValue(const T& val) { throwIfSatisfied(); mStorage = val; satisfy(Status::ValueSet); }
        void setValue(T&& val) { throwIfSatisfied(); mStorage = std::move(val); satisfy(Status::ValueSet); }
    private:
        T mStorage;
    };

    template <>
    class CARBON_API PromiseImpl<void> : virtual public PromiseImplBase {
    public:
        void get();
        void setValue();
    };

    template <class T>
    class Promise {
    public:
        Promise(PromiseImpl<T>* r) { r->incCount(); x = r; }
        Promise(const Promise&) = delete;
        Promise& operator = (const Promise&) = delete;
        Promise(Promise&& r) : x(r.x) { r.x = nullptr; }
        Promise& operator = (Promise&& r) { x = r.x, r.x = nullptr; return *this; }
        ~Promise() { if (x) if (x->decCount() == 1) delete x; }
        auto get() { return x->get(); }
        void wait() { x->wait(); }
        template <class Rep, class Period>
        bool waitFor(const std::chrono::duration<Rep, Period>& rt) { return x->waitFor(rt); }
        template<class Clock, class Duration>
        bool waitUntil(const std::chrono::time_point<Clock, Duration>& tt) { return x->waitUntil(tt); }
    private:
        PromiseImpl<T>* x;
    };

    template <>
    class Promise<void> {
    public:
        Promise(PromiseImpl<void>* r) { r->incCount(); x = r; }
        Promise(const Promise&) = delete;
        Promise& operator = (const Promise&) = delete;
        Promise(Promise&& r) : x(r.x) { r.x = nullptr; }
        Promise& operator = (Promise&& r) { x = r.x, r.x = nullptr; return *this; }
        ~Promise() { if (x) if (x->decCount() == 1) delete x; }
        void get() { x->get(); }
        void wait() { x->wait(); }
        template <class Rep, class Period>
        bool waitFor(const std::chrono::duration<Rep, Period>& rt) { return x->waitFor(rt); }
        template<class Clock, class Duration>
        bool waitUntil(const std::chrono::time_point<Clock, Duration>& tt) { return x->waitUntil(tt); }
    private:
        PromiseImpl<void>* x;
    };

}
