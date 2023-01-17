#pragma once

#include "NonCopyable.h"
#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"

#include <bits/c++config.h>
#include <functional>


class Acceptor : NonCopyable
{
public:
    using NewConnectionCallback = std::function<void(int , const InetAddress&)>;

    Acceptor(EventLoop *loop, const InetAddress &listenaddr, bool reuseport);

    ~Acceptor();

    void setNewConnetionCallback(NewConnectionCallback cb){newConnectionCallback_ = cb;}

    bool listenning() const { return listenning_; }

    void listen();

private:
    // void handleNewConnection()//flag
    // listenfd 有事件发生 有新用户连接 , 回调设置的newConnectinCallback()
    void handleNewConnection(TimeStamp newConnectionTime);

    EventLoop *loop_; // baseLoop   mainloop
    Socket acceptSocket_;
    Channel acceptChannel_;

    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};

