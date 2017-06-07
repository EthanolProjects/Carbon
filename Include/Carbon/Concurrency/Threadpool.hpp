#pragma once
#include "Carbon/Config/Config.hpp"
#include <memory>

namespace Carbon {
    struct CARBON_API Work {
        Work();
        Work(Work&&) = delete;
        Work(const Work&) = delete;
        Work& operator = (Work&&) = delete;
        Work& operator = (const Work&) = delete;
        virtual ~Work();
        virtual void main() noexcept = 0;
    };

    class CARBON_API Threadpool {
    public:
        Threadpool();
        Threadpool(Threadpool&&) = delete;
        Threadpool(const Threadpool&) = delete;
        Threadpool& operator = (Threadpool&&) = delete;
        Threadpool& operator = (const Threadpool&) = delete;
        virtual ~Threadpool();
        virtual void submitOnce(Work* task) = 0;
        virtual size_t getConcurrencyLevel() const;
        static Threadpool& getDefault() noexcept;
        static std::unique_ptr<Threadpool> create();
    };
}
