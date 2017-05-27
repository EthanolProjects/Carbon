#pragma once
#include <atomic>
#include <exception>
#include <functional>
namespace Carbon {
    template <class T> class LightPromise;
    template <class T> class LightFuture;
    namespace {
        enum class S : int { P, S, V, E, O }; //Pending, Setting, ValueSet, ExceptionSet, Obtained
        template <class T>
        struct _AssociatedStatus {
            std::atomic<S> stat{ S::P };
            std::atomic_int refCount{ 0 };
            std::exception_ptr exception;
            void* then;
            T retVal;
        };
        template <>
        struct _AssociatedStatus<void> {
            std::atomic<S> stat{ S::P };
            std::atomic_int refCount{ 0 };
            std::exception_ptr exception;
            void* then;
        };
        template <class T>
        class _AssocStatControl {
        public:
            _AssocStatControl(): mAssocStat(new _AssociatedStatus<T>()) {}
            _AssocStatControl(const _AssocStatControl& r) :mAssocStat(r.mAssocStat) { 
                if (mAssocStat) mAssocStat->refCount.fetch_add(1);
            }
            _AssocStatControl& operator=(const _AssocStatControl& r) {
                mAssocStat = r.mAssocStat;
                if (mAssocStat)  mAssocStat->refCount.fetch_add(1);
            }
            _AssocStatControl(_AssocStatControl&& r) :mAssocStat(r.mAssocStat) {
                r.mAssocStat = nullptr;
            }
            _AssocStatControl& operator=(const _AssocStatControl& r) {
                mAssocStat = r.mAssocStat;
                r.mAssocStat = nullptr;
            }
            ~_AssocStatControl() {
                if (mAssocStat)
                    if (mAssocStat->refCount.fetch_sub(1) == 1)
                        delete mAssocStat;
            }
            _AssociatedStatus<T>& operator * () noexcept { return *mAssocStat; }
            const _AssociatedStatus<T>& operator * () const noexcept { return *mAssocStat; }
            _AssociatedStatus<T>* operator -> () noexcept { return mAssocStat; }
            const _AssociatedStatus<T>* operator -> () const noexcept { return mAssocStat; }
        private:
            _AssociatedStatus<T>* mAssocStat{ nullptr };
        };
    }
    template <class T> 
    class LightFuture {
    public:
        friend class LightPromise<T>;
        bool isReady() const noexcept { 
            return mAssocStat->stat == S::V || mAssocStat->stat == S::E; 
        }
        std::pair<T, bool> tryGet() const { 
            if (mAssocStat->stat == S::E) 
                std::rethrow_exception(mAssocStat->exception); 
            return std::make_pair<T, bool>(mAssocStat->retVal, mAssocStat->stat == S::V; 
        }
        auto share() const { return LightFuture(*this); }
    private:
        LightFuture(const _AssocStatControl<T>& r) : mAssocStat(r) {}
        _AssocStatControl<T> mAssocStat;
    };
    template <void>
    class LightFuture {
    public:
        friend class LightPromise<void>;
        bool isReady() const noexcept {
            return mAssocStat->stat == S::V || mAssocStat->stat == S::E;
        }
        bool tryGet() const {
            if (mAssocStat->stat == S::E)
                std::rethrow_exception(mAssocStat->exception);
            return mAssocStat->stat == S::V;
        }
        auto share() const { return LightFuture(*this); }
    private:
        LightFuture(const _AssocStatControl<void>& r) : mAssocStat(r) {}
        _AssocStatControl<void> mAssocStat;
    };

}
