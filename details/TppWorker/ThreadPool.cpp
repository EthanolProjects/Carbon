#include "ThreadPool.hpp"
#include <algorithm>

namespace Carbon {

    namespace TppDetail{
        LockFreeList::LockFreeList():m_head(nullptr){}
        LockFreeList::~LockFreeList(){
            Node* it; 
            while (it=m_head) {m_head=it->next; delete it;}
        }

        void LockFreeList::addTask(std::unique_ptr<Task>&& task , const std::weak_ptr<TaskSetState>& signal , size_t id){
            auto newHead=std::make_unique<Node>();
            newHead->next=m_head;
            newHead->task=std::make_unique<TaskInfo>(std::move(task),signal,id);

            while(!m_head.compare_exchange_weak(newHead->next,newHead.get()));
            newHead.release();
        }

        std::unique_ptr<TaskInfo> LockFreeList::getTask(){
            if(m_head==nullptr)return nullptr;
            Node *node=nullptr,*next;
            do
            {
                node = m_head.load();
                next = node ? node->next : nullptr;
            }
            while(!m_head || !m_head.compare_exchange_weak(node,next));
            auto task=node?std::move(node->task):nullptr;
            delete node;
            return task;
        }

        void PoolThread::setSource(LockFreeList& source){
            m_source = &source;
            m_flag = true;
            m_thread = std::thread([this] {runThread(); });
        }

        void PoolThread::runThread(){
            std::unique_ptr<TaskInfo> task;
            while (m_flag){
                while (m_flag && !(task = m_source->getTask()))
                    std::this_thread::yield();
                if (!m_flag)break;
                task->run();
                task.reset();
            }
        }

        PoolThread::~PoolThread(){
            m_flag=false;
            if(m_thread.joinable())
                m_thread.detach();
        } 

        TaskInfo::TaskInfo(std::unique_ptr<Task>&& task,const std::weak_ptr<TaskSetState>& signal,size_t id)
            :m_task(std::move(task)),m_signal(signal),m_id(id){}

        void TaskInfo::run(){
            m_task->run();
        }
        
        TaskInfo::~TaskInfo(){
            if(!m_signal.expired()){
                m_signal.lock()->finished();
            }
        }
    }
    TaskSetState::TaskSetState(size_t size):m_count(0),m_size(size){}

    void TaskSetState::wait() const{
        while(m_size != m_count)std::this_thread::yield();
    }

    void TaskSetState::finished(){
        ++m_count;
    }

    ThreadPool::ThreadPool(size_t num){
        if(num==0)num=std::thread::hardware_concurrency();
        m_threads = std::make_unique<TppDetail::PoolThread[]>(10);
        for (size_t i = 0; i < num; ++i)
            m_threads[i].setSource(m_source);
    }

}
