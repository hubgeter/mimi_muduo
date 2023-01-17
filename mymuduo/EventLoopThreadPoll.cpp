#include "EventLoopThreadPoll.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include<vector>

//线程里绑定的loop都是栈上的对象 
EventLoopThreadPoll::EventLoopThreadPoll(EventLoop* baseloop,int numThread)
    :baseLoop_(baseloop)
    ,started_(false)
    ,numThread_(numThread)
    ,next_(0)
{

}

EventLoopThreadPoll::~EventLoopThreadPoll()
{
}

void EventLoopThreadPoll::start(const ThreadInitCallbal &cb)
{
    started_ = true;

    for(int i = 0;i<numThread_;i++)
    {
        EventLoopThread* t = new EventLoopThread(cb);
        thread_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
    if (numThread_ == 0 && cb )
    {
        cb(baseLoop_);
    }
}


EventLoop* EventLoopThreadPoll::getNextLoop()
{
    EventLoop*loop = baseLoop_;

    if (!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;    
        if (next_ >= loops_.size())
            next_ = 0;
    }
    return loop;
}


std::vector<EventLoop*> EventLoopThreadPoll::getAllLoops()
{
    if(loops_.empty())
    {
        return std::vector<EventLoop *>(1, baseLoop_);
    }
    else
    { 
        return loops_;
    }
}


//通过给相应loop的 weakupfd_ 数据 来唤醒subreactor 
//acceptor 运行在 mainreator 里 ， 分发