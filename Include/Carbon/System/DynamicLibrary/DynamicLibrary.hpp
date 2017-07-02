#pragma once
#include "Carbon/Config/Config.hpp"
#include <cstdint>
#include <memory>

namespace Carbon {
    using FarCall = void(*)();
    class CARBON_API DynamicLibrary {
    public:
        DynamicLibrary(const char* path, bool needSuffixAdded = false);
        ~DynamicLibrary();
        template <class FuncPT>
        FuncPT get(const char* name) { return FuncPT(getImpl(name)); }
    protected:
        void* mLibrary;
        FarCall getImpl(const char* name);
    };

}
