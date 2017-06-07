// ============================================================================
// Copyright (c) 2010 Faustino Frechilla
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// @file lock_free_queue.h
/// @brief Definition of a circular array based lock-free queue
/// See http://www.codeproject.com/Articles/153898/Yet-another-implementation-of-a-lock-free-circular
/// for more info
///
/// @author Faustino Frechilla
/// @history
/// Ref  Who                 When         What
///      Faustino Frechilla  11-Jul-2010  Original development
///      Faustino Frechilla  08-Aug-2014  Support for single producer through LOCK_FREE_Q_SINGLE_PRODUCER #define
///      Faustino Frechilla  11-Aug-2014  LOCK_FREE_Q_SINGLE_PRODUCER removed. Single producer handled in template
///      Faustino Frechilla  12-Aug-2014  inheritance (specialisation) based on templates.
///      Faustino Frechilla  10-Aug-2015  Ported to c++11. Removed volatile keywords (using std::atomic)
/// @endhistory
/// 
// ============================================================================

#pragma once

#include <atomic>
#include <cstdint>

#define LOCK_FREE_Q_DEFAULT_SIZE 8192

template <class ELEM_T, uint32_t Q_SIZE = LOCK_FREE_Q_DEFAULT_SIZE>
class ArrayLockFreeQueue {
public:
    inline bool full() const noexcept;
    inline uint32_t size() const noexcept;
    bool pop(ELEM_T &a_data) noexcept;
    bool push(const ELEM_T &a_data) noexcept;
private:
    ELEM_T m_theQueue[Q_SIZE];
    std::atomic<uint32_t> mWriteIndex{ 0u };
    std::atomic<uint32_t> mReadIndex{ 0u };
    std::atomic<uint32_t> mMaximumReadIndex{ 0u };
    static inline uint32_t countToIndex(uint32_t a_count) noexcept;
};

///////////////////////////////////////////////////////////////////////////////
//
//                           Implementation
//
///////////////////////////////////////////////////////////////////////////////
#include <cassert>
template <class ELEM_T, uint32_t Q_SIZE>
inline uint32_t ArrayLockFreeQueue<ELEM_T, Q_SIZE>::countToIndex(uint32_t a_count) noexcept {
    // if Q_SIZE is a power of 2 this statement could be also written as 
    // return (a_count & (Q_SIZE - 1));
    return (a_count % Q_SIZE);
}

template <class ELEM_T, uint32_t Q_SIZE>
inline uint32_t ArrayLockFreeQueue<ELEM_T, Q_SIZE>::size() const noexcept {
    uint32_t currentWriteIndex = mMaximumReadIndex.load();
    uint32_t currentReadIndex = mReadIndex.load();

    // let's think of a scenario where this function returns bogus data
    // 1. when the statement 'currentWriteIndex = mMaximumReadIndex' is run
    // mMaximumReadIndex is 3 and mReadIndex is 2. Real size is 1
    // 2. afterwards this thread is preemted. While this thread is inactive 2 
    // elements are inserted and removed from the queue, so mMaximumReadIndex 
    // is 5 and mReadIndex 4. Real size is still 1
    // 3. Now the current thread comes back from preemption and reads mReadIndex.
    // currentReadIndex is 4
    // 4. currentReadIndex is bigger than currentWriteIndex, so
    // m_totalSize + currentWriteIndex - currentReadIndex is returned, that is,
    // it returns that the queue is almost full, when it is almost empty
    //
    if (countToIndex(currentWriteIndex) >= countToIndex(currentReadIndex)) {
        return (currentWriteIndex - currentReadIndex);
    }
    else {
        return (Q_SIZE + currentWriteIndex - currentReadIndex);
    }
}

template <class ELEM_T, uint32_t Q_SIZE>
inline bool ArrayLockFreeQueue<ELEM_T, Q_SIZE>::full() const noexcept {
    uint32_t currentWriteIndex = mWriteIndex.load();
    uint32_t currentReadIndex = mReadIndex.load();
    return (countToIndex(currentWriteIndex + 1) == countToIndex(currentReadIndex));
}

template <class ELEM_T, uint32_t Q_SIZE>
bool ArrayLockFreeQueue<ELEM_T, Q_SIZE>::push(const ELEM_T &a_data) noexcept {
    uint32_t currentWriteIndex;

    do {
        currentWriteIndex = mWriteIndex.load();

        if (countToIndex(currentWriteIndex + 1) == countToIndex(mReadIndex.load())) {
            // the queue is full
            return false;
        }
        // There is more than one producer. Keep looping till this thread is able 
        // to allocate space for current piece of data
        //
        // using compare_exchange_strong because it isn't allowed to fail spuriously
        // When the compare_exchange operation is in a loop the weak version
        // will yield better performance on some platforms, but here we'd have to
        // load mWriteIndex all over again
    } while (!mWriteIndex.compare_exchange_strong(
        currentWriteIndex, (currentWriteIndex + 1)));

    // Just made sure this index is reserved for this thread.
    m_theQueue[countToIndex(currentWriteIndex)] = a_data;

    // update the maximum read index after saving the piece of data. It can't
    // fail if there is only one thread inserting in the queue. It might fail 
    // if there is more than 1 producer thread because this operation has to
    // be done in the same order as the previous CAS
    //
    // using compare_exchange_weak because they are allowed to fail spuriously
    // (act as if *this != expected, even if they are equal), but when the
    // compare_exchange operation is in a loop the weak version will yield
    // better performance on some platforms.
    while (!mMaximumReadIndex.compare_exchange_weak(
        currentWriteIndex, (currentWriteIndex + 1))) {
        // this is a good place to yield the thread in case there are more
        // software threads than hardware processors and you have more
        // than 1 producer thread
        // have a look at sched_yield (POSIX.1b)
        //sched_yield();
    }

    return true;
}

template <class ELEM_T, uint32_t Q_SIZE>
bool ArrayLockFreeQueue<ELEM_T, Q_SIZE>::pop(ELEM_T &a_data) noexcept {
    uint32_t currentReadIndex;

    do {
        currentReadIndex = mReadIndex.load();

        // to ensure thread-safety when there is more than 1 producer 
        // thread a second index is defined (mMaximumReadIndex)
        if (countToIndex(currentReadIndex) == countToIndex(mMaximumReadIndex.load())) {
            // the queue is empty or
            // a producer thread has allocate space in the queue but is 
            // waiting to commit the data into it
            return false;
        }

        // retrieve the data from the queue
        a_data = m_theQueue[countToIndex(currentReadIndex)];

        // try to perfrom now the CAS operation on the read index. If we succeed
        // a_data already contains what mReadIndex pointed to before we 
        // increased it
        if (mReadIndex.compare_exchange_strong(currentReadIndex, (currentReadIndex + 1))) {
            // got here. The value was retrieved from the queue. Note that the
            // data inside the m_queue array is not deleted nor reseted
            return true;
        }

        // it failed retrieving the element off the queue. Someone else must
        // have read the element stored at countToIndex(currentReadIndex)
        // before we could perform the CAS operation        

    } while (1); // keep looping to try again!  
    assert(0); // Something went wrong. it shouldn't be possible to reach here
               // Add this return statement to avoid compiler warnings
    return false;
}


