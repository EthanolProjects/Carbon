#include "Log.hpp"
namespace Carbon {
    Logger::Entry::Entry(const char *func, const char *file, std::int32_t line, std::int32_t sev, Logger &hst) :
        mHost(hst), mLine(line), mFile(file), mFunc(func), mSev(sev) {
    }

    Logger::Entry::~Entry() {
        mHost.recordEntry(mFunc, mFile, mLine, mSev, mStreamBuf);
    }

    void Logger::recordEntry(const char *func, const char *file, std::int32_t line, std::int32_t sev, std::stringstream &buffer) {
        //TODO: STUB! Implement This!
    }

}