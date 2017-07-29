#pragma once
#include "Promise.hpp"
#include "Synchronization.hpp"
#include <memory>

namespace Carbon {
    class CARBON_API AssocTpWorkStatShared : public virtual PromiseImplBase {
    public:
        AssocTpWorkStatShared();
        ~AssocTpWorkStatShared();
        void submit();
        bool runWork();
    protected:
        virtual bool xRunWork();
    };

    template <class T>
    class AssocTpWorkStat : public PromiseImpl<T>, public AssocTpWorkStatShared {
    };

}
