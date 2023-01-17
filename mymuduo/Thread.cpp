#include "Thread.h"
#include "CurrentThread.h"

#include <semaphore.h>
#include <string>
// std::atomic<int>  Thread::numThread_(0); 
//thread 对象记录的是线程的状况 
Thread::Thread(ThreadFunc func)
    :started_(false)
    ,joined_(false)
    ,tid_(0)
    ,func_(std::move(func))
    ,name_("thread")
{

}
void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem,false,0);
    thread_ = std::shared_ptr<std::thread>(new std::thread(
        [&](){
            tid_ = CurrentThread::tid();//获取线程tid
            name_ += std::to_string(tid_);
            sem_post(&sem);
            func_();//开启一个线程 ，专门执行该线程

        }
    ));
    //这里必须等待获取上面创建的线程的tid值
    sem_wait(&sem);

}
void Thread::join()
{
    joined_ = true;
    thread_->join();
}

Thread::~Thread()
{
    if (started_ && (!joined_))
    {   
        thread_->detach();
    }
}
void setDefaultName()
{
    //thread %d  numThread 
    
}