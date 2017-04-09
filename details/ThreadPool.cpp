#include "ThreadPool.hpp"
#include <algorithm>

namespace Carbon {

    namespace TppDetail{
        LockFreeList::LockFreeList():m_head(nullptr){}
        LockFreeList::~LockFreeList(){
            Node* it; 
            while (it=m_head) {m_head=it->next; delete it;}
        }

        void LockFreeList::addTask(const TaskInfo& task){
            auto newHead=std::make_unique<Node>();
            newHead->next=m_head;
            newHead->task=std::make_unique<TaskInfo>(std::move(task));

            while(!m_head.compare_exchange_weak(newHead->next,newHead.get()));
            newHead.release();
        }

        std::unique_ptr<TaskInfo> LockFreeList::getTask(){
            if(m_head==nullptr)return nullptr;
            Node* node=nullptr;
            while(!m_head.compare_exchange_weak(node,m_head->next));
            auto task=node->task->release();
            delete node;
            return task;
        }

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

    bool TaskSetState::test() const{
        return std::any_of(m_state.cbegin(),m_state.cend(),[](bool x){return !x;});
    }
    void TaskSetState::wait() const{
        while(test)std::this_thread::yield();
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

}
