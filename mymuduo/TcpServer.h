#pragma once

#include "NonCopyable.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "TimeStamp.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "EventLoopThreadPoll.h"

#include <functional>
#include <iostream>

class TcpConnection;
class Buffer;

using namespace std::placeholders;
/*
    用户使用muduo 编写服务器程序
    对外服务器编程使用的类
*/

// mianloop
// acceptor socket
//          listen
// acceptChannel
// poller

// acceptor::handleread <<-- set

// acceptor_ ==> acceptChannel --> enablereading --> pooler
//-->handelread --> newconnetioncallback <- setnewconnectioncallback

//默认连接回调函数
void defaultConnectionCallback(const TcpConnectionPtr &);

// 默认消息回调
void defaultMessageCallback(const TcpConnectionPtr &, Buffer *, TimeStamp);

class TcpServer : NonCopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    enum option{
        KNoReusePort,//0 
        KResuePort //1
    };

    /// @brief Tcpserver 创建 acceptor运行在mainloop , EventLoopThreadPoll
    /// @param loop : mainloop    
    /// @param listenaddr :监听的端口与地址
    /// @param op : 复用端口
    TcpServer(EventLoop *loop, const InetAddress &listenaddr, std::string name = std::string("server"), option op = KNoReusePort);



    ~TcpServer();

    //设置线程数量
    void setThreadnum(int numThreads){threadPoll_->setThreadNum(numThreads);}

    //设置线程初始化回调函数
    void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }
    //设置连接建立/断开回调函数
    void setConnectionCallback(ConnectionCallback cb){connectionCallback_ = cb;}
    //设置消息到达回调函数
    void setMessageCallback(MessageCallback cb){messageCallback_ = cb;}
    //设置写完成回调函数函数
    void setWriteComplateCallback(WriteCompleteCallback cb){writeCompleteCallback_ = cb;}

    // 开启服务器监听
    void start();

private:

    void newConnection(int sockfd, const InetAddress &peeraddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    

    EventLoop *loop_; // baseloop/mainloop 
    const std::string ipport_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_; // 监听连接事件
    std::shared_ptr<EventLoopThreadPoll> threadPoll_;

    ConnectionCallback connectionCallback_;       // 新连接的回调
    MessageCallback messageCallback_;             // 读写消息回调
    WriteCompleteCallback writeCompleteCallback_; // 消息发送完成的回调

    ThreadInitCallback threadInitCallback_; // loop 初始化的回调
    std::atomic<bool> started_;

    using ConnecttionMap = std::map<std::string, TcpConnectionPtr>;
    ConnecttionMap connecttionMap_;
};