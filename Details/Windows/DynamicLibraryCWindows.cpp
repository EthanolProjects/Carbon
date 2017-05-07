#include "DynamicLibrary.hpp"
#include "CarbonTargetWindows.hpp"
#include <stdexcept>
#ifdef CARBON_TARGET_WINDOWS
namespace CarbonWindows {
    using namespace Carbon;
    class DyLibWindows : public DynamicLibrary {
    public:
        DyLibWindows(const std::string& path) {
            mLibrary = LoadLibraryA(path.c_str());
            if (mLibrary == nullptr) {
                throw std::runtime_error("Library Not Found!");
            }
        }
        ~DyLibWindows() {
            if (mLibrary)
                FreeLibrary(mLibrary);
        }
    protected:
        void* getImpl(const char* name) override {
            return reinterpret_cast<void*>(GetProcAddress(mLibrary, name));
        }
    private:
        HINSTANCE mLibrary;
    };
}
namespace Carbon {
    using namespace CarbonWindows;
    std::unique_ptr<DynamicLibrary> DynamicLibrary::create(const char * path, bool needSuffixAdded) {
        return (needSuffixAdded ? std::make_unique<DyLibWindows>(std::string(path)) : 
            std::make_unique<DyLibWindows>(std::string(path) + ".dll"));
    }
}
#endif
