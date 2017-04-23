#include "Dll.hpp"
#include <stdexcept>
namespace Carbon {
#ifdef CARBON_TARGET_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
    class DynamicLibrary::DyImpl {
    public:
        DyImpl(const char* path) {
            mLibrary = LoadLibraryA(path);
            if (mLibrary == nullptr){
                throw std::runtime_error("Library Not Found!");
            }
        }
        ~DyImpl() {
            if (mLibrary)
                FreeLibrary(mLibrary);
        }
        void* getFunction(const char* name) {
            return reinterpret_cast<void*>(GetProcAddress(mLibrary, name));
        }
    private:
        HINSTANCE mLibrary;
    };
#elif CARBON_TARGET_LINUX || CARBON_TARGET_APPLE
#include <dlfcn.h>
    class DynamicLibrary::DyImpl {
    public:
        DyImpl(const char* path) {
            mLibrary = dlopen(path,RTLD_NOW);
            auto error = dlerror();
            if (mLibrary == nullptr || error){
                throw std::runtime_error(static_cast<char*>(error));
            }
        }
        ~DyImpl() {
            if (mLibrary)
                dlclose(mLibrary);
        }
        void* getFunction(const char* name) {
            return dlsym(mLibrary, name);
        }
    private:
        void* mLibrary;
    };
#endif

    DynamicLibrary::DynamicLibrary(): mImpl(nullptr) {}
    DynamicLibrary::~DynamicLibrary() { if (mImpl) delete mImpl; }
    DynamicLibrary::DynamicLibrary(DynamicLibrary&& rhs) {
        if (rhs.mImpl != nullptr) {
            mImpl = rhs.mImpl;
            rhs.mImpl = nullptr;
        }
    }

    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& rhs) {
        if (mImpl) {
            delete mImpl; mImpl = nullptr;
        }
        if (rhs.mImpl != nullptr) {
            mImpl = rhs.mImpl;
            rhs.mImpl = nullptr;
        }
        return *this;
    }

    DynamicLibrary::DynamicLibrary(const char * path, bool needSuffixAdded) {
        std::string pathFull = std::string(path);
        if (needSuffixAdded) {
#if defined CARBON_TARGET_WINDOWS
            pathFull += ".dll";
#elif defined CARBON_TARGET_LINUX
            pathFull += ".so";
#elif defined CARBON_TARGET_APPLE
            pathFull += ".dylib";
#endif
        }
        try {
            mImpl = new DyImpl(pathFull.c_str());
        }
        catch (std::exception&) {
            mImpl = nullptr;
            throw;
        }
    }

    void* DynamicLibrary::loadCFunctionImpl(const char* name) {
        return mImpl->getFunction(name);
    }

    namespace {
        std::string abiTransform(const char* name, DynamicLibrary::ABI abi) {
            switch (abi) {
            case DynamicLibrary::ABI::MSVC:
                break;
            case DynamicLibrary::ABI::Itanium:
                break;
            }
            return std::string();
        }
    }

    void* DynamicLibrary::loadCXXFunctionImpl(const char* name, ABI abi) {
        if (abi == ABI::Local) {
#ifdef _MSC_VER
            abi = ABI::MSVC;
#else
            abi = ABI::Itanium;
#endif
        }
        return mImpl->getFunction(abiTransform(name, abi).c_str());
    }
}
