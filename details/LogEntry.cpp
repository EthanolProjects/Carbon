#include "Log.hpp"
#include <chrono>

namespace Carbon {
    Logger::Logger() = default;

    Logger::Entry::Entry(const char *func, const char *file, std::int32_t line, std::int32_t sev, Logger &hst) :
        mHost(hst), mLine(line), mFile(file), mFunc(func), mSev(sev) {
    }

    Logger::Entry::~Entry() {
        mHost.flush(mHost.format(mFunc , mFile , mLine , mSev , mStreamBuf.str()));
    }

}