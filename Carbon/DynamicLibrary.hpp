#pragma once
#include "Config.hpp"
#include <cstdint>
#include <memory>
namespace Carbon {
    class CARBON_API DynamicLibrary {
    public:
        enum class ABI : uint8_t {
            Local, MSVC, Itanium
        };
        virtual ~DynamicLibrary() {}
        template <class FuncPT>
        FuncPT get(const char* name) { return FuncPT(getImpl(name)); }
        std::unique_ptr<DynamicLibrary> create(const char* path, bool needSuffixAdded = false);
    protected:
        virtual void* getImpl(const char* name) = 0;
    };

}