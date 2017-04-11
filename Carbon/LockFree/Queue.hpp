#pragma once

#include <atomic>
namespace Carbon {
    template <class T>
    struct LinkList {
        T data;
        std::atomic<LinkList<T>*> next;
    };

    // NOTE: THIS IS VERY EXPERIMENTAL! USE WITH CAUTION!
    template <class T>
    class LockFreeQueue {
    public:
        LockFreeQueue();
        ~LockFreeQueue();
        bool isEmpty();
        int size();
        void push(T&& src);
        bool pop(T& dst);
    private:
        LinkList<T> *mHead;
        LinkList<T> *mTail;
        std::atomic_int mSize;
    };

    template <class T>
    LockFreeQueue<T>::LockFreeQueue() : mHead(nullptr), mTail(new LinkList<T>), mSize(0) {
        mHead = mTail;
        mTail->next = nullptr;
    }

    template <class T>
    LockFreeQueue<T>::~LockFreeQueue() {
        T acc;
        while (size() > 0)
            pop(acc);
    }

    template <class T>
    void LockFreeQueue<T>::push(T&& t) {
        LinkList<T>* newVal = new LinkList<T>, * nullval = nullptr;
        newVal->data = std::forward<T>(t);
        newVal->next = nullptr;
        while (!std::atomic_compare_exchange_strong(&mTail->next, &nullval, newVal));
        //std::atomic_compare_exchange_strong(&mTail, mTail, newVal);
        ++mSize;
    }

    template <class T>
    bool LockFreeQueue<T>::pop(T& dst) {
        LinkList<T> *p = nullptr;
        do {
            p = mHead->next;
            if (!p) return false;
        } while (!std::atomic_compare_exchange_strong(&(mHead->next), &p, (p->next.load())));
        --mSize;
        std::swap(dst, p->data);
        delete p;
        return true;
    }

    template <class T>
    bool LockFreeQueue<T>::isEmpty() {
        return (mSize == 0);
    }

    template <class T>
    int LockFreeQueue<T>::size() {
        return static_cast<int>(mSize.load());
    }
}
