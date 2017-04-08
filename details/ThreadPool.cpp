#include "ThreadPool.hpp"
#include <algorithm>

namespace Carbon {

    namespace TppDetail{
        
        PoolThread::PoolThread(LockFreeList& source)
            :m_source(source),m_flag(true),m_thread([this]{runThread();}){}

        void PoolThread::runThread(){
            std::unique_ptr<TaskInfo> task;
            while(m_flag){
                while(!(task=m_source.getTask()))
                    std::this_thread::yield();
                task->run();
                task->reset();
            }
        }

        PoolThread::~PoolThread(){
            m_flag=false;
            m_thread.join();
        } 

        TaskInfo::TaskInfo(Task&& task,const std::weak_ptr<TaskSetState>& signal,size_t id)
            :m_task(task),m_signal(signal),m_id(id){}

        void TaskInfo::run(){
            m_task->run();
        }
        
        TaskInfo::~TaskInfo(){
            if(!m_signal.expired()){
                m_signal.lock()->finished(m_id);
            }
        }
    }
    TaskSetState::TaskSetState(size_t num):m_state(num,false){}

    void TaskSetState::wait() const{
        while(std::any_of(m_state.cbegin(),m_state.cend(),[](bool x){return !x;}))
            std::this_thread::yield();
    }

    void TaskSetState::finished(size_t id){
        m_state[id]=true;
    }

    ThreadPool::ThreadPool(size_t num){
        if(num)num=std::thread::hardware_concurrency();
        m_threads.reserve(num)
        for(size_t i=0;i<num;++i)
            m_threads.emplace_back(m_queue);
    }

    std::shared_ptr<TaskSetState> ThreadPool::addTasks(const TaskSet& tasks){
        auto res=std::make_shared<TaskSetState>(tasks.size());
        size_t id=0;
        for(Task&& task:tasks){
            m_source.addTask({task,res,id});
            ++id;
        }
        return res;
    }
}