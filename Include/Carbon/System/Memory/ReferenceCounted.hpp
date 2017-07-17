#pragma once
#include "Carbon/Config/Config.hpp"
#include <atomic>
namespace Carbon {
    class CARBON_API ReferenceCounted {
    public:
        ReferenceCounted(ReferenceCounted&&) = delete;
        ReferenceCounted(const ReferenceCounted&) = delete;
        ReferenceCounted& operator = (ReferenceCounted&&) = delete;
        ReferenceCounted& operator = (const ReferenceCounted&) = delete;
        int incCount() noexcept;
        int decCount() noexcept;
    protected:
        ReferenceCounted();
        virtual ~ReferenceCounted();
    private:
        std::atomic_int x;
    };
}