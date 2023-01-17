#include "EventLoop.h"
#include "Logger.h"
#include "TimeStamp.h"

#include<string.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory> 

//       eventloop  => poller.poll()
// channellist      poller
//                  channelmap<fd,channel*> epollfd  、


//防止一个线程创建多个eventLoop
//__thread --> thread local 
__thread EventLoop* t_loopInThisThread = nullptr;
//定义默认的poller IO复用超时事件
const int KPollTimeMs = 10000;

//创建weakupfd 用来唤醒subreactor处理新来的channel  
static int createEventfd()
{
    int evtfd = eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if (evtfd < 0  )
    {
    }
    return evtfd;
}
EventLoop::EventLoop()
    :looping_(false)
    ,quit_(false)
    ,callingPendingFuncors_(false)
    ,threatid_(CurrentThread::tid())
    ,poller_(Poller::newDefaultPoller(this))
    ,wakeupfd_(createEventfd())
    ,weakupChannel_(new Channel(this, wakeupfd_))
    ,currentActiveChannl_(nullptr)
{
    if(t_loopInThisThread)
    {
        //log_fault
        LOG_FATAL("repeat eventloop");
    }
    else
    {
        t_loopInThisThread = this;
    }     
    
    //设置wakeupfd的事件类型，发生事件的回调函数
    weakupChannel_->setReadCallback(std::bind(&EventLoop::handleWeakup,this));
    //每一个eventloop都将监听wakeupchannel 的epollin
    weakupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    weakupChannel_->disableall();
    weakupChannel_->remove();
    close(wakeupfd_);
    t_loopInThisThread = nullptr;
}
void EventLoop::handleWeakup()
{
    uint64_t one = 1;
    ssize_t sz = read(wakeupfd_,&one,sizeof one);
}
void EventLoop::loop()
{
    looping_ = true;
    quit_= false;


    while(!quit_)
    {
        activeChannels_.clear();
        // 监听两类fd 1.cientfd   2.wakeupfd
        pollReturnTime_ = poller_->poll(KPollTimeMs,&activeChannels_);
        
        for(Channel*channel :activeChannels_)
        {
            channel->handleevent(pollReturnTime_);
        }

        //mainloop 实现注册一个回调cb (需要subloop来执行)  
        //weakup subloop 后执行下面的方法，执行之前注册的回调操作
        doPendingFunctors();
    }
    looping_= false;
}
void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())//其他线程调用quit 
    {
        wakeup();
    }
}
//mainloop 和 subloop通过eventfd通信，而不是生产者消费者队列
//runinloop queueinloop 的调用可以不是当前线程
//结果是运行在调用方对应的loop 
//loop所属线程可能处在别的状态：epool_wait 执行回调...
//调用线程唤醒loop所属线程
void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}
void EventLoop::queueInLoop(Functor cb)
{  
    {
        std::unique_lock<std::mutex> lck(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    //callpeningfunc   当前loop正在执行回调，但loop又有了新的回调
    if (!isInLoopThread() || callingPendingFuncors_)
    {
        //唤醒loop所在线程，执行cb
        wakeup();
    }
}
//向wakeupfd_写一个数据
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t sz = write(wakeupfd_,&one,sizeof one);
    if ( sz != sizeof one){
        //log_error
    }
}

void EventLoop::updateChannel(Channel*channel ){
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel* channel){
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channnel){
    return poller_->hasChannel(channnel);
}

void EventLoop::doPendingFunctors()
{   
    std::vector<Functor> functors;
    callingPendingFuncors_ = true;
    {
        std::unique_lock<std::mutex> lck(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(const Functor&functor:functors)
    {
        functor();
    }
    callingPendingFuncors_ = false;
}