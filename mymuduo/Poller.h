#pragma once 

#include "Channel.h"
#include "NonCopyable.h"
#include "TimeStamp.h"


#include <vector>
#include <map>

class Channel;
class EventLoop;

class Poller : NonCopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller(){};
    
    //给所有io复用保留统一的接口 
    virtual TimeStamp poll(int timeoutMs,ChannelList* activeChannelLists) = 0;
    virtual void updateChannel(Channel* ) = 0;
    virtual void removeChannel(Channel* ) = 0;

    //判断参数是否在当前的poller当中
    bool hasChannel(Channel* channel) const;
    
    //拿到默认的io复用的具体实现
    static Poller* newDefaultPoller(EventLoop*loop);

protected:
    using ChannelMap = std::map<int,Channel*>;
    ChannelMap channels_;


private:

    EventLoop * ownerLoop_;
};