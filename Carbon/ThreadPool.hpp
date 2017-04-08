#include <thread>
#include <atomic>
#include <memory>
#include "Task.hpp"

namespace Carbon {

    namespace TppDetail{
		class ThreadPool;
        class TaskSetState;
        class LockFreeList;
        class CARBON_API PoolThread final{
		public:
            PoolThread(LockFreeList& source);
            ~PoolThread();
		private:
			void runThread();
			std::thread m_thread;
			LockFreeList& m_source;
            bool m_flag;
        };
		
		class CARBON_API TaskInfo final{
		public:
            TaskInfo(Task&& task,const std::weak_ptr<TaskSetState>& signal,size_t id);
			void run();
			~TaskInfo();
		private:
			Task m_task;
			std::weak_ptr<TaskSetState> m_signal;
            size_t m_id;
		}
		
		class CARBON_API LockFreeList final{
		public:
		void addTask(const TaskInfo& task);
		std::unique_ptr<TaskInfo> getTask();
		private:
        };
    }
	
	class CARBON_API TaskSet final{
	public:
		auto begin();
		auto end();
		size_t size() const;
	private:
	};
		
	class CARBON_API TaskSetState final{
    friend class TppDetail::TaskInfo;
	public:
		TaskSetState(size_t num);
        void wait() const;
        /*
        void wait_until() const;
        void wait_for() const;
        */
	private:
        std::vector<bool> m_state;
        void finished(size_t id);
	};
	
    class CARBON_API ThreadPool final{
    public:
        ThreadPool(size_t num=0);
		std::shared_ptr<TaskSetState> addTasks(const TaskSet& tasks);
    private:
        std::vector<TppDetail::PoolThread> m_threads;
		TppDetail::LockFreeList m_queue;
    };

}
