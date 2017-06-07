#include "Carbon/System/DynamicLibrary/DynamicLibrary.hpp"
#include <stdexcept>
#ifdef CARBON_TARGET_POSIX
#include <dlfcn.h>
namespace CarbonPosix {
    using namespace Carbon;
    class DyLibPosix : public DynamicLibrary {
    public:
        DyLibPosix(const std::string& path) {
            mLibrary = dlopen(path.c_str(), RTLD_NOW);
            auto error = dlerror();
            if (mLibrary == nullptr || error) {
                throw std::runtime_error(static_cast<char*>(error));
            }
        }
        ~DyLibPosix() {
            if (mLibrary)
                dlclose(mLibrary);
        }
    protected:
        void* getImpl(const char* name) override {
            return dlsym(mLibrary, name);
        }
    private:
        void* mLibrary;
    };
}
namespace Carbon {
    using namespace CarbonPosix;
    std::unique_ptr<DynamicLibrary> DynamicLibrary::create(const char * path, bool needSuffixAdded) {
        return std::make_unique<DyLibPosix>(std::string(path));
    }
}
#endif
