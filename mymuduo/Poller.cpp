#include "Poller.h"


Poller::Poller(EventLoop* loop)
    :ownerLoop_(loop)
{

}

bool Poller::hasChannel(Channel* channel) const 
{
    auto it = this->channels_.find(channel->fd()); 
    return  it != this->channels_.end() && it->second == channel;
}

#include "EpollPoller.h"

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    return new EPollPoller(loop);
}
