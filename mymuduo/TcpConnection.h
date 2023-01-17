#pragma once 


#include "Channel.h"
#include "Socket.h"
#include "EventLoop.h"
#include "NonCopyable.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "Logger.h"

#include <atomic>
#include <unistd.h>
#include <iostream>
#include <netdb.h>

class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer*,TimeStamp)>;




class TcpConnection:NonCopyable 
    ,public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, int sockfd,std::string name,InetAddress localaddr,InetAddress peerAddr);
    
    ~TcpConnection();


    
    /*
    设置关于一条连接上的一系列回调函数
    */
    void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = cb; }
    void setMessageCallback(MessageCallback cb) { messageCallback_ = cb; }
    void setWritecomplateCallback(WriteCompleteCallback cb) { writeCompleteCallback_ = cb; }
    void setCloseCallback(CloseCallback cb) { closeCallback_ = cb; }

    
    void setTcpNoDelay(bool on) { socket_->setTcpNoDelay(on); }

    //开启读
    void startRead() { loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, this)); }
    //关闭读
    void stopRead() { loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this)); }
    bool isReading() const { return reading_; }


    //可以在非io线程调用 发送数据到 peeraddr 
    void send(const std::string &message);

    //关闭写端
    void shutdown();


    //连接建立
    void connnectEstablished();
    
    //销毁连接
    void connectDestory();

    std::string name() const {return name_;}
    EventLoop *getLoop() const  { return loop_; }
    bool isConnected() const { return state_ == KConnected; }

private:
    
    enum State{ 
        KConnecting,
        KConnected,
        KDisconnecting,
        KDisconnected    
    };

    std::atomic<int> state_;
    void set_State(State s) { state_ = s; }

    void sendInLoop(std::string message);
    void shutdownInLoop();

    bool reading_;
    void startReadInLoop();
    void stopReadInLoop();

    /*
    用于关联channel  channel进行回调
*/
    //处理pool上可读事件  读出数据回调 messageCallback_ 
    void handleRead(TimeStamp recvtime);
    //处理pool上可写事件  写完则回调writeCompleteCallback_
    void handleWrite();
    //处理pool上关闭事件  
    void handleClose();
    //处理pool上错误事件
    void handleError();

    /*
        channel -> headle... -> ...Callback  
        回调 用于 tcpserver , tcpclient
    */
    ConnectionCallback  connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;

    EventLoop *loop_; // subloop
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;


    const InetAddress localAddr_;
    const InetAddress peerAddr_;

    std::string name_;

    Buffer inbuffer_;
    Buffer outbuffer_;
};