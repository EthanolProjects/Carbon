#include "Concurrency.hpp"

namespace Carbon {
    TaskGroupFuture::TaskGroupFuture(size_t size) :mLast(size) {}
    TaskGroupFuture::~TaskGroupFuture() {
        wait();
    }
    size_t TaskGroupFuture::setException(std::exception_ptr exc, size_t size) {
        std::lock_guard<std::mutex> lock(mMutex);
        mExceptions.emplace_back(exc);
        return mLast -= size;
    }

    size_t TaskGroupFuture::finish(size_t size) {
        return mLast -= size;
    }

    void TaskGroupFuture::wait() const {
        size_t last = mLast, current;
        using clock = std::chrono::high_resolution_clock;
        auto time = clock::now();
        while (mLast) {
            current = mLast;
            if (last != current) {
                auto now = clock::now();
                auto wait = (now - time)*current / (last - current);
                std::this_thread::sleep_for(wait / 100);
                time = now;
                last = current;
            }
            else std::this_thread::yield();
        }
    }

    void TaskGroupFuture::catchExceptions(const std::function<void(std::function<void()>)>& catchFunc) {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto&& exc : mExceptions) {
            catchFunc([&exc] {std::rethrow_exception(std::move(exc)); });
        }
        mExceptions.clear();
    }

    IntegerRange::IntegerRange(size_t begin, size_t end) :mBegin(begin), mEnd(end) {}
    IntegerRange::IntegerRange(const IntegerRange & rhs)
        : mBegin(rhs.mBegin.load()), mEnd(rhs.mEnd) {}
    IntegerRange IntegerRange::cut(size_t atomic) {
        size_t lb = mBegin.load();
        size_t end;
        do end = std::min(lb + atomic, mEnd);
        while (!mBegin.compare_exchange_weak(lb, end));
        return { lb,end };
    }
    size_t IntegerRange::size() const {
        return mEnd > mBegin ? mEnd - mBegin : 0;
    }
    void IntegerRange::forEach(const std::function<void(size_t)>& callable) const {
        size_t begin = mBegin.load();
        while (begin < mEnd) { callable(begin); ++begin; }
    }
    void IntegerRange::forEach(const std::function<void(IntegerRange)>& callable) const {
        callable(*this);
    }
}
