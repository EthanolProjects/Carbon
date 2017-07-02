#include "Carbon/System/DynamicLibrary/DynamicLibrary.hpp"
#include "../../Targets/CarbonTargetWindows.hpp"
#include <stdexcept>
#ifdef CARBON_TARGET_WINDOWS
namespace Carbon {
    DynamicLibrary::DynamicLibrary(const char * path, bool needSuffixAdded) {
        std::string spath(path);
        if (needSuffixAdded)
            spath += ".dll";
        mLibrary = LoadLibraryA(spath.c_str());
        if (mLibrary == nullptr) {
            throw std::runtime_error("Library Not Found!");
        }
    }

    DynamicLibrary::~DynamicLibrary () {
        if (mLibrary)
            FreeLibrary(reinterpret_cast<HMODULE>(mLibrary));
    }

    FarCall DynamicLibrary::getImpl(const char* name) {
        return reinterpret_cast<FarCall>(GetProcAddress(reinterpret_cast<HMODULE>(mLibrary), name));
    }

}
#endif
