#include "Carbon/System/DynamicLibrary/DynamicLibrary.hpp"
#include <stdexcept>
#ifdef CARBON_TARGET_POSIX
#include <dlfcn.h>
namespace Carbon {
    DynamicLibrary::DynamicLibrary(const char * path, bool needSuffixAdded) {
        std::string spath(path);
        if (needSuffixAdded)
            spath += ".so";
        mLibrary = dlopen(spath.c_str(), RTLD_NOW);
        auto error = dlerror();
        if (mLibrary == nullptr || error) {
            throw std::runtime_error(static_cast<char*>(error));
        }
    }

    DynamicLibrary::~ DynamicLibrary() {
        if (mLibrary)
            dlclose(mLibrary);
    }

    FarCall DynamicLibrary::getImpl(const char* name) {
        return reinterpret_cast<FarCall>(dlsym(mLibrary, name));
    }
}
#endif
