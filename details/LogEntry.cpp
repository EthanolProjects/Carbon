#include "Log.hpp"
#include <chrono>

namespace Carbon {
    Logger::Logger() = default;

    Logger::Entry::Entry(const char *func, const char *file, std::int32_t line, std::int32_t sev, Logger &hst) :
        mHost(hst), mLine(line), mFile(file), mFunc(func), mSev(sev) {
    }

    Logger::Entry::~Entry() {
        mHost.recordEntry(mFunc, mFile, mLine, mSev, mStreamBuf);
    }

    void Logger::recordEntry(const char *func, const char *file, std::int32_t line, std::int32_t sev, std::stringstream &buffer) {
        buffer << "\nMore Info:\n    Func:" << func << "\n    File:" << file << "\n    Line:" << line << "\n    Sev:" << sev<<"\n\n";
        flush(buffer);
    }

}