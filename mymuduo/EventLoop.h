#pragma once 

#include "NonCopyable.h"
#include "Poller.h"
#include "Channel.h"
#include "TimeStamp.h"
#include "CurrentThread.h"
#include "Channel.h"


#include <vector>
#include <atomic>
#include <memory>
#include <mutex>
#include <functional>

class Channel;
class Poller;
/*
    one loop per thread

    时间循环类

    包含
        channel
        poller(epoll的抽象)

*/
class EventLoop
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    //开启事件循环
    void loop();
    //退出事件循环
    void quit();

    TimeStamp pollReturnTime() const {return pollReturnTime_;}

    // 在当前loop线程中执行cb ，可以有非loop对应的线程调用
    void runInLoop(Functor cb);

    //把cb放到队列中，唤醒loop所在的线程，执行cb
    void queueInLoop(Functor cb);
    //唤醒loop所在的线程
    void wakeup();

    void updateChannel(Channel* );
    void removeChannel(Channel* );
    bool hasChannel(Channel* );

    bool isInLoopThread()const {return threatid_ == CurrentThread::tid();}
    pid_t getTid() const { return threatid_; }

private:
    void handleWeakup();//weak up 

    void doPendingFunctors();// 执行回调

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;//标识退出loop循环

    const pid_t threatid_;//每个线程运行自己的eventloop
    TimeStamp pollReturnTime_;//poller返回事件channels的时间点
    std::unique_ptr<Poller> poller_;

    /*
    每一个subreactor 监听一个weakupfd  mainloop与subloop可以通过 该fd交互/唤醒
    */
    int wakeupfd_;
    std::unique_ptr<Channel> weakupChannel_;


    using ChannelList = std::vector<Channel*>;
    ChannelList activeChannels_;
    Channel* currentActiveChannl_;

    std::atomic<bool> callingPendingFuncors_;//标识当前loop是否有需要执行的回调操作
    std::vector<Functor> pendingFunctors_;//存储loop需要执行的所有回调操作
    std::mutex mutex_;//保证上面vector的线程安全
};