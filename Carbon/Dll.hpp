#pragma once
#include "Config.hpp"
#include <cstdint>
namespace Carbon {
    class CARBON_API DynamicLibrary {
    public:
        DynamicLibrary();
        DynamicLibrary(const char* path, bool needSuffixAdded = false);
        DynamicLibrary(DynamicLibrary&&);
        DynamicLibrary& operator=(DynamicLibrary&&);
        DynamicLibrary(const DynamicLibrary&) = delete;
        DynamicLibrary& operator=(const DynamicLibrary&) = delete;
        ~DynamicLibrary();
        template <class FuncPT>
        FuncPT loadCFunction(const char* name) {
            return FuncPT(loadCFunctionImpl(name));
        }
        enum class ABI : uint8_t {
            Local, MSVC, Itanium
        };
        template <class FuncPT>
        FuncPT loadCXXFunction(const char* name, ABI abi = ABI::Local) {
            return FuncPT(loadCXXFunctionImpl(name, ABIType));
        }
    private:
        void* loadCFunctionImpl(const char* name);
        void* loadCXXFunctionImpl(const char* name, ABI ABI);
        class DyImpl;
        DyImpl* mImpl;
    };

}