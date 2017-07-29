#pragma once
#include "Carbon/System/Memory/ReferenceCounted.hpp"
namespace Carbon {
    class CARBON_API Scheduleable : public ReferenceCounted {
    public:
    };

    class CARBON_API Scheduler {
    public:
        Scheduler(Scheduler&&) = delete;
        Scheduler(const Scheduler&) = delete;
        Scheduler& operator = (Scheduler&&) = delete;
        Scheduler& operator = (const Scheduler&) = delete;
    protected:
        Scheduler() {}
        virtual ~Scheduler() {}
    };

}
