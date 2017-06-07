#pragma once
#include "Carbon/Config/Config.hpp"
#include <cstdint>
#include <memory>

namespace Carbon {
    class CARBON_API DynamicLibrary {
    public:
        virtual ~DynamicLibrary() {}
        template <class FuncPT>
        FuncPT get(const char* name) { return FuncPT(getImpl(name)); }
        static std::unique_ptr<DynamicLibrary> create(const char* path, bool needSuffixAdded = false);
    protected:
        virtual void* getImpl(const char* name) = 0;
    };

}