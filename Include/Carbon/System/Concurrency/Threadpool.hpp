#pragma once
#pragma warning (push)
#pragma warning (disable: 4250)
#include "Carbon/Config/Config.hpp"
#include "Promise.hpp"
#include "Synchronization.hpp"
#include <memory>

namespace Carbon {
#if defined(XCARBON_TARGET_WINDOWS)
    class CARBON_API AssocTpWorkStatShared : public virtual PromiseImplBase {
    public:
        AssocTpWorkStatShared();
        ~AssocTpWorkStatShared();
        void submit();
        bool runWork();
    protected:
        virtual bool xRunWork();
        void* mWorkHandle;
    };
#else
    class CARBON_API AssocTpWorkStatShared : public virtual PromiseImplBase {
    public:
        AssocTpWorkStatShared();
        ~AssocTpWorkStatShared();
        void submit();
        bool runWork();
    protected:
        virtual bool xRunWork();
    };
#endif

    template <class T>
    class AssocTpWorkStat : public PromiseImpl<T>, public AssocTpWorkStatShared {
    };

}
#pragma warning (pop)
