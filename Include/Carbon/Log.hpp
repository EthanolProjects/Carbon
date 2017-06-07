#pragma once
#include "Config.hpp"
#include <vector>
#include <memory>
#include <cstdint>
#include <sstream>
namespace Carbon {
    class Logger {
    public:
        virtual ~Logger() {}
        virtual void recordLog(const char* func, const char* file, std::int32_t line, std::int32_t sev, const char* discription) = 0;
    };

    class LogEntry {
    public:
        LogEntry(const char* func, const char* file, std::int32_t line, std::int32_t sev, Logger& host) : 
            mHost(host), mFunc(func), mFile(file), mLine(line), mSev(mSev) {}
        ~LogEntry() { mHost.recordLog(mFunc, mFile, mLine, mSev, mStream.str().c_str()); }
        template <class T>
        LogEntry& operator << (const T& item) { mStream << item; return *this; }
    private:
        Logger& mHost;
        const char* mFunc;
        const char* mFile;
        std::int32_t mLine, mSev;
        std::stringstream mStream;
    };

    class LogHub: public Logger {
    public:
        LogHub() {}
        template <class HostT, class ...Ts>
        void addLogger(Ts&&... args) { mHosts.push_back(std::make_unique<HostT>(std::forward<Ts>(args)...)); }
        void recordLog(const char* func, const char* file, std::int32_t line, std::int32_t sev, const char* discription) override {
            for (auto&& host : mHosts)
                host->recordLog(func, file, line, sev, discription);
        }
    private:
        std::vector<std::unique_ptr<Logger>> mHosts;
    };

#define CARBON_LOG_SEV(logger, sev) logger(__func__  , __FILE__, __LINE__, sev)
}
