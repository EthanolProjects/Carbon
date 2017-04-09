#include <thread>
#include <atomic>
#include <memory>
#include <chrono>
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
			LockFreeList();
			~LockFreeList();
			void addTask(const TaskInfo& task);
			std::unique_ptr<TaskInfo> getTask();
		private:
			struct Node final{
				std::unique_ptr<TaskInfo> task=nullptr;
				Node* next=nullptr;
			}
			std::atomic<Node*> m_head;
        };
    }
		
	class CARBON_API TaskSetState final{
    friend class TppDetail::TaskInfo;
	public:
		TaskSetState(size_t num);
        void wait() const;
        template< class Clock, class Duration >
		bool wait_until(const std::chrono::time_point<Clock,Duration>& timeout) const;
		{
			using clock=std::chrono::system_clock;
			while(test()){
				if(clock::now()>=timeout)return false;
				std::this_thread::yield();
			}
			return true;
		}
        template< class Rep, class Period >
		bool wait_for(const std::chrono::duration<Rep,Period>& timeout) const{
			using clock=std::chrono::system_clock;
			auto time=clock::now()+timeout;
			return wait_until(time);
		}
	private:
		bool test() const;
        std::vector<bool> m_state;
        void finished(size_t id);
	};
	
    class CARBON_API ThreadPool final{
    public:
        ThreadPool(size_t num=0);
		template<typename TaskSet>
		std::shared_ptr<TaskSetState> addTasks(const TaskSet& tasks){
        	auto res=std::make_shared<TaskSetState>(tasks.size());
        	size_t id=0;
        	for(Task&& task:tasks){
        	    m_source.addTask({task,res,id});
        	    ++id;
        	}
        	return res;
		}
    private:
        std::vector<TppDetail::PoolThread> m_threads;
		TppDetail::LockFreeList m_queue;
    };

}
