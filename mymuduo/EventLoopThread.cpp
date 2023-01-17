#include "EventLoopThread.h"
// #include ""

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadfunc, this))
    , callback_(cb)

{

}
EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_!=nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}


EventLoop* EventLoopThread::startLoop(){
    thread_.start();
    {
        std::unique_lock<std::mutex>lck(mutex_);
        // cond_.wait_for(lck, loop != nullptr);
        cond_.wait(lck, [&]()->bool{ return loop_ != nullptr;});
    }
    return loop_;
}

void EventLoopThread::threadfunc()
{
    EventLoop loop;

    if (callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lck(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();

    //退出
    std::unique_lock<std::mutex>lck(mutex_);
    loop_ = nullptr;
}