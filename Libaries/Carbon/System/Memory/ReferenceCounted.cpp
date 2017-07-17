#include "Carbon/System/Memory/ReferenceCounted.hpp"
namespace Carbon {
    ReferenceCounted::ReferenceCounted() :x(0) {}

    ReferenceCounted::~ReferenceCounted() = default;

    int ReferenceCounted::incCount() noexcept { return x.fetch_add(1); }

    int ReferenceCounted::decCount() noexcept { return x.fetch_sub(1); }

}
