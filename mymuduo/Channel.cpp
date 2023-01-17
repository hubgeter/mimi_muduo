#include "Channel.h"
#include<sys/epoll.h>

const int Channel::KnoneEvent = 0;
const int Channel::KreadEvent = EPOLLIN | EPOLLPRI;
const int Channel::KwriteEvent = EPOLLOUT ;


Channel::Channel(EventLoop* loop,int fd)
    :loop_(loop)
    ,fd_(fd)
    ,events_(0)
    ,revents_(0)
    ,index_(-1)
    ,tied_(false)
{
}

Channel::~Channel()
{


}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_= true;
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    loop_->removeChannel(this);
}

//提供给eventllop调用 来处理对应的回调事件 + set_revents 设置对应的状态 
void Channel::handleevent(TimeStamp receiveTime)
{
    if (tied_)
    {
        std::shared_ptr<void> guard  = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else 
    {
        handleEventWithGuard(receiveTime);
    }

}


void Channel::handleEventWithGuard(TimeStamp receiveTime)
{   
    //对端正常关闭（程序里close()，shell下kill或ctr+c），触发EPOLLIN和EPOLLRDHUP
    //是不触发EPOLLERR和EPOLLHUP。
    //带外数据，只触发EPOLLPRI
    if (revents_&  EPOLLERR )
    {
        if (errorcallback_)
        {
            errorcallback_();
        }     
    }
    if (revents_& (EPOLLIN|EPOLLPRI|EPOLLRDHUP) )
    {
        if (readcallback_)
        {
            readcallback_(receiveTime);
        }
    }
    if(revents_&EPOLLOUT)
    {
        if (writecallback_)
        {
            writecallback_();
        }
    }
    if ( (revents_& EPOLLHUP ) && !(revents_&EPOLLIN))
    {
        if (closecallback_)
        {
            closecallback_();
        }
    }
}


