#include <thread>
#include <atomic>
#include <memory>
#include <chrono>
#include <vector>
#include "Task.hpp"

namespace Carbon {
    class CARBON_API ThreadPool;
    class CARBON_API TaskSetState;
    namespace TppDetail {
        class CARBON_API LockFreeList;
        class CARBON_API PoolThread final {
        public:
            void setSource(LockFreeList& source);
            ~PoolThread();
        private:
            void runThread();
            std::thread m_thread;
            LockFreeList* m_source;
            bool m_flag;
        };

        class CARBON_API TaskInfo final {
        public:
            TaskInfo(std::unique_ptr<Task>&& task, const std::weak_ptr<TaskSetState>& signal, size_t id);
            void run();
            ~TaskInfo();
        private:
            std::unique_ptr<Task> m_task;
            std::weak_ptr<Carbon::TaskSetState> m_signal;
            size_t m_id;
        };

        class CARBON_API LockFreeList final {
        public:
            LockFreeList();
            ~LockFreeList();
            void addTask(std::unique_ptr<Task>&& task, const std::weak_ptr<TaskSetState>& signal, size_t id);
            std::unique_ptr<TaskInfo> getTask();
        private:
            struct Node final {
                std::unique_ptr<TaskInfo> task = nullptr;
                Node* next = nullptr;
            };
            std::atomic<Node*> m_head;
        };
    }

    class CARBON_API TaskSetState final {
        friend class TppDetail::TaskInfo;
    public:
        TaskSetState(size_t size);
        void wait() const;
        template< class Clock, class Duration >
        bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout) const {
            using clock = std::chrono::system_clock;
            while (m_size != m_count) {
                if (clock::now() >= timeout)return false;
                std::this_thread::yield();
            }
            return true;
        }
        template< class Rep, class Period >
        bool wait_for(const std::chrono::duration<Rep, Period>& timeout) const {
            using clock = std::chrono::system_clock;
            auto time = clock::now() + timeout;
            return wait_until(time);
        }
    private:
        std::atomic_size_t m_count, m_size;
        void finished();
    };

    class CARBON_API ThreadPool final {
    public:
        ThreadPool(size_t num = 0);
        std::shared_ptr<TaskSetState> addTask(std::unique_ptr<Task>&& task) {
            auto res = std::make_shared<TaskSetState>(1);
            m_source.addTask(std::move(task), std::weak_ptr<TaskSetState>(res), 0);
            return res;
        }
        template<typename TaskSet>
        std::shared_ptr<TaskSetState> addTasks(TaskSet& tasks) {
            auto res = std::make_shared<TaskSetState>(std::size(tasks));
            size_t id = 0;
            for (std::unique_ptr<Task>& task : tasks) {
                m_source.addTask(std::move(task), res, id);
                ++id;
            }
            return res;
        }
    private:
        std::unique_ptr<TppDetail::PoolThread[]> m_threads;
        TppDetail::LockFreeList m_source;
    };

}
