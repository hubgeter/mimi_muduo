#pragma once 

#include "NonCopyable.h"
#include "EventLoop.h"
#include "Thread.h"

#include <functional>
#include <mutex>
#include<condition_variable>

/*
one loop per thread 
*/
class EventLoopThread:NonCopyable 
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback());
    ~EventLoopThread();

    /*
    启动线程并且返回新线程中eventlloop对象地址
    */
    EventLoop *startLoop();

private:
    void threadfunc();

    EventLoop * loop_;
    bool exiting_;
    
    Thread thread_;

    std::mutex mutex_;
    std::condition_variable cond_;
    
    ThreadInitCallback callback_;
};
