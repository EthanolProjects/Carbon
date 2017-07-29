#include "Carbon/System/Concurrency/Promise.hpp"
#include <cassert>
namespace Carbon {
    namespace {
        // Promise Internal Status Namesapce
    }

    PromiseImplBase::PromiseImplBase() :mEvent(false), mStatus(Status::Waiting), mExceptionPointer() {}

    PromiseImplBase::~PromiseImplBase() {}

    void PromiseImplBase::wait() { mEvent.wait(); }

    void PromiseImplBase::abandon() { 
        throwIfSatisfied();
        satisfy(Status::Broken); 
    }

    bool PromiseImplBase::isSatisfied() const noexcept {
        auto s = mStatus.load(); 
        return s != Status::Waiting;
    }

    void PromiseImplBase::catchCurrentException() { setException(std::current_exception()); }

    void PromiseImplBase::satisfy(Status s) noexcept { mStatus.store(s); mEvent.set(); }

    void PromiseImplBase::getImpl() {
        if (mStatus.load() == Status::Retrived)
            throw PromiseException(PromiseException::Code::PromisedResultAlreadyRetrieved);
        wait();
        satisfy(Status::Retrived);
        if (mStatus.load() == Status::ExceptionSet) {
            assert(mExceptionPointer);
            std::rethrow_exception(mExceptionPointer);
        }
    }

    void PromiseImplBase::throwIfSatisfied() {
        if (isSatisfied())
            throw PromiseException(PromiseException::Code::PromiseAlreadySatisfied);
    }

    void PromiseImplBase::setException(std::exception_ptr e) {
        throwIfSatisfied();
        mExceptionPointer = e; 
        satisfy(Status::ExceptionSet);
    }

    PromiseException::PromiseException(Code c) : mCode(c) {}

    PromiseException::~PromiseException() {}

    PromiseException::PromiseException(const PromiseException &) = default;

    PromiseException & PromiseException::operator=(const PromiseException &) = default;

    PromiseException::Code PromiseException::errorCode() const noexcept {  return mCode; }

    char const * PromiseException::what() const noexcept {
        constexpr const char* exceptionMessageTable[]{
            "Promise Already Satisfied",
            "Promise Broken",
            "Promised Result Already Retrieved"
        };
        return exceptionMessageTable[static_cast<size_t>(mCode)];
    }

    void PromiseImpl<void>::setValue() {
        throwIfSatisfied();
        satisfy(Status::ValueSet);
    }

    void PromiseImpl<void>::get() { getImpl(); }

}
