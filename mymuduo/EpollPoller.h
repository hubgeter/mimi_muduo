#pragma once

#include "TimeStamp.h"
#include "Poller.h"

#include <vector>
#include<sys/epoll.h>

/*
epoll的使用
*/
class EPollPoller:public Poller
{
public:
    EPollPoller(EventLoop*);    
    ~EPollPoller() override;

    //epoll_wait 
    TimeStamp poll(int timeousMs,ChannelList*activeChannels) override;

    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;


private:
    static const int KInitEventListSize = 16;
    //填写活跃的连接
    void fillActiveChannels(int numEvents,ChannelList* activeChannels)const;

    //更新channel通道
    void update(int operation,Channel* channel);


    using EventList = std::vector<epoll_event>;
    int epollfd_;
    EventList events_;
};