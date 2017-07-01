#pragma once
#pragma warning (push)
#pragma warning (disable: 4250)
#include "Carbon/Config/Config.hpp"
#include "LightFuture.hpp"
#include "Synchronization.hpp"
#include <memory>

namespace Carbon {
#if defined(CARBON_TARGET_WINDOWS)
    class CARBON_API AssocTpWorkStatShared : public virtual AssocStateBase {
    public:
        AssocTpWorkStatShared();
        ~AssocTpWorkStatShared();
        void submit();
        bool runWork();
        void wait() override;
    protected:
        void xGet();
        virtual bool xRunWork();
        bool xWaitFor(long long milli) override;
        void* mWorkHandle;
        System::Event mEvent;
        std::exception_ptr mExceptionPointer;
    };
#else
    class CARBON_API AssocTpWorkStatShared : public virtual AssocStateBase {
    public:
        AssocTpWorkStatShared();
        ~AssocTpWorkStatShared();
        void submit();
        bool runWork();
        void wait() override;
    protected:
        void xGet();
        virtual bool xRunWork();
        bool xWaitFor(long long milli) override;
        System::Event mEvent;
        std::exception_ptr mExceptionPointer;
    };
#endif

    template <class T>
    class AssocTpWorkStat : public AssocState<T>, public AssocTpWorkStatShared {
    public:
        void set(const T& r) override { mValue = r; }
        T get() override { xGet(); return mValue; }
    private:
        T mValue;
    };

    template <>
    class AssocTpWorkStat<void> : public AssocState<void>, public AssocTpWorkStatShared {
    public:
        void set() override {}
        void get() override { xGet(); }
    };

}
#pragma warning (pop)
