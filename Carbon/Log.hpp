#pragma once
#include "Config.hpp"
#include <cstdint>
#include <sstream>
namespace Carbon {
    class CARBON_API Logger{
    public:
        // Default Constructable
        Logger();
        // Non-Copyable
        Logger(const Logger&) = delete;
        Logger& operator = (const Logger&) = delete;
        // Log Entry and Log Function
        class CARBON_API Entry {
        public:
            Entry(const char* func, const char* file, std::int32_t line, std::int32_t sev, Logger& hst);
            ~Entry();
            Entry(Entry&&) = default;
            template <class T>
            Entry& operator << (T&& rhs) { mStreamBuf << rhs; return *this; }
        private:
            Logger& mHost;
            const char* mFunc;
            const char* mFile;
            std::int32_t mLine, mSev;
            std::stringstream mStreamBuf;
        };
        template <class Sev>
        auto operator()(const char* func, const char* file, std::int32_t line, Sev sev = Sev()) {
            return Entry(func, file, line, static_cast<std::int32_t>(sev), *this);
        }
    private:
        void recordEntry(const char* func, const char* file, std::int32_t line, std::int32_t sev, std::stringstream& buffer);
        virtual std::string format(const char* func , const char* file , std::int32_t line , std::int32_t sev , const std::string& str) const= 0;
        virtual void flush(const std::string& str)=0;
    };
#define CARBON_LOG_SEV(logger, sev) logger(__func__  , __FILE__, __LINE__, sev)
}
