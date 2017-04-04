#pragma once
#include <sstream>
namespace Carbon {
    class Logger{
    public:
        // Default Constructable
        Logger() = default;
        // Non-Copyable
        Logger(const Logger&) = delete;
        Logger& operator = (const Logger&) = delete;
        // Log Entry and Log Function
        class Entry {
        public:
            Entry(const char* func, const char* file, int line, Logger& hst);
            ~Entry();
            template <class T>
            Entry& operator << (T&& rhs) { mStringBuf << rhs; return *this; }
        private:
            Logger& mHost;
            std::stringstream mStreamBuf;
        };
        template <class Sev>
        auto operator()(const char* file, int line, Sev sev = Sev()) { return logSev(static_cast<unsigned long>(sev)); }
    private:
        void recordEntry(const char* func, const char* file, int line, std::stringstream& buffer);
        Entry logSev(const char* func, const char* file, int line, unsigned long sevLev);
    };
#define CARBON_LOG_SEV(logger, sev) logger(__FUNC__, __FILE__, __LINE__, sev)
}
