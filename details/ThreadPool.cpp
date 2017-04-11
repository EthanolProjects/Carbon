#include "ThreadPool.hpp"
#include <algorithm>

namespace Carbon {
    namespace TppDetail {
        LockFreeList::LockFreeList() :m_head(nullptr) {}
        LockFreeList::~LockFreeList() {
            Node* it;
            while (it = m_head) { m_head = it->next; delete it; }
        }

        void LockFreeList::addTask(std::unique_ptr<Task>&& task) {
            auto newHead = std::make_unique<Node>();
            newHead->next = m_head;
            newHead->task = std::move(task);
            while (!m_head.compare_exchange_weak(newHead->next, newHead.get()));
            newHead.release();
        }

        std::unique_ptr<Task> LockFreeList::getTask() {
            if (m_head == nullptr)return nullptr;
            Node *node = nullptr, *next;
            do {
                node = m_head.load();
                next = node ? node->next : nullptr;
            } while (!m_head || !m_head.compare_exchange_weak(node, next));
            auto task = node ? std::move(node->task) : nullptr;
            delete node;
            return task;
        }

        void PoolThread::setSource(LockFreeList& source) {
            m_source = &source;
            m_flag = true;
            m_thread = std::thread([this]() { runThread(); });
        }

        void PoolThread::runThread() {
            std::unique_ptr<Task> task;
            while (m_flag) {
                while (m_flag && !(task = m_source->getTask()))
                    std::this_thread::yield();
                if (!m_flag)break;
                task->run();
                task.reset();
            }
        }

        PoolThread::~PoolThread() {
            m_flag = false;
            if (m_thread.joinable())
                m_thread.detach();
        }
    }
   
    ThreadPool::ThreadPool(size_t num) {
        if (num == 0)
            num = std::thread::hardware_concurrency();
        m_threads = std::make_unique<TppDetail::PoolThread[]>(num);
        for (size_t i = 0; i < num; ++i)
            m_threads[i].setSource(m_source);
    }

}