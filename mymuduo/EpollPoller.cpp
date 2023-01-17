#include "EpollPoller.h"
#include "TimeStamp.h"
#include "Logger.h"


#include<unistd.h>
#include<string.h>

//channel 未添加到poller中
const int KNew = -1;
//channel 添加到poller中
const int KAdded = 1;
//channel 从poller中删除
const int KDeleted = 2;

EPollPoller::EPollPoller(EventLoop*loop)
    :Poller(loop)
    ,epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    ,events_(KInitEventListSize)
{
}

EPollPoller::~EPollPoller()
{
    close(epollfd_);
}

//添加/删除 channel  
void EPollPoller::updateChannel(Channel* channel)
{

    const int index = channel->index();
    //channel 初始化为 knew -1 
    if (index == KNew || index == KDeleted)
    {   
        if (index == KNew)
        {
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(KAdded);
        update(EPOLL_CTL_ADD,channel);
    }else
    {
        //已经在poller上注册过了  mod / del 
        int fd = channel->fd();
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL,channel);
            channel->set_index(KDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD,channel);
        }
    }

}


void EPollPoller::removeChannel(Channel* channel) 
{
    int fd = channel->fd();
    channels_.erase(fd);

    int index = channel->index();
    if (index == KAdded)
    {
        update(EPOLL_CTL_DEL,channel);
    }
    channel->set_index(KNew);
}
void EPollPoller::update(int operation,Channel* channel)
{
    epoll_event event;
    memset(&event,0,sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();

    if ( ::epoll_ctl(epollfd_,operation,fd,&event) < 0 )
    {
        //... error 
        if(operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_del error ,fd = %d", fd);
        }
        else
        {
            LOG_FATAL("update error fd:%d",fd);
        }
    }


}
TimeStamp EPollPoller::poll(int timeoutMs,ChannelList*activeChannels)
{
    int numEvents = epoll_wait(epollfd_,events_.data(),static_cast<int>(events_.size()),timeoutMs);


    if(numEvents > 0 )
    {
        
        fillActiveChannels(numEvents,activeChannels);
        // if (numEvents == events_.size())
        // {
        //     events_.resize(events_.size()*2);
        // }
    }
    else if (numEvents == 0 )
    {
    }
    else
    {   
        if(errno != EINTR)
        {
            LOG_INFO("epoll_wait ");
        }
    }

    return TimeStamp::now();
}
void EPollPoller::fillActiveChannels(int numEvents,ChannelList*activeChannels) const 
{
    for(int i = 0;i<numEvents;i++){
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);

        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}