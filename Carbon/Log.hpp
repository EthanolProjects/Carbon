#pragma once
#include <cstdint>
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
            Entry(const char* func, const char* file, std::int32_t line, std::int32_t sev, Logger& hst);
            ~Entry();
            template <class T>
            Entry& operator << (T&& rhs) { mStringBuf << rhs; return *this; }
        private:
            Logger& mHost;
            const char* mFunc;
            const char* mFile;
            std::int32_t mLine, mSev;
            std::stringstream mStreamBuf;
        };
        template <class Sev>
        auto operator()(const char* func, const char* file, std::int32_t line, Sev sev = Sev()) {
            return Entry(func, file, line, static_cast<std::int32_t>(sev));
        }
    private:
        void recordEntry(const char* func, const char* file, std::int32_t line, std::int32_t sev, std::stringstream& buffer);
    };
#define CARBON_LOG_SEV(logger, sev) logger(__FUNC__, __FILE__, __LINE__, sev)
}
