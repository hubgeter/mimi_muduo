// #include "tcpserver.h"
#include "TcpServer.h"

void defaultConnectionCallback(const TcpConnectionPtr & conn)
{
    LOG_INFO("conn = %s defaultConnectionCallback" ,conn->name().c_str());
}
void defaultMessageCallback(const TcpConnectionPtr & conn , Buffer *, TimeStamp)
{
    LOG_INFO("conn = %s defaultMessageCallback",conn->name().c_str());
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenaddr, std::string name , option op )
    : loop_(loop)
    , ipport_(listenaddr.toIpPort())
    , name_(name)
    , acceptor_(new Acceptor(loop, listenaddr,op))
    , threadPoll_(new EventLoopThreadPoll(loop))
    , connectionCallback_(defaultConnectionCallback)
    , messageCallback_(defaultMessageCallback)
{
    acceptor_->setNewConnetionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));//设置有新连接时的回调函数
}

TcpServer::~TcpServer()
{
    for(auto& item:connecttionMap_)
    {
        TcpConnectionPtr conn(item.second);
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestory, conn));
    }

}

void TcpServer::start()
{
    threadPoll_->start(threadInitCallback_);
    loop_->runInLoop(
        std::bind(&Acceptor::listen, acceptor_.get()));
}

void TcpServer::newConnection(int sockfd, const InetAddress &peeraddr)
{
    EventLoop *ioloop = threadPoll_->getNextLoop();

    std::string connName(InetAddress(getLocalAddr(sockfd)).toIpPort() + "-" + peeraddr.toIpPort());
    TcpConnectionPtr conn(new TcpConnection(ioloop, sockfd, connName, InetAddress(getLocalAddr(sockfd)), peeraddr));

    connecttionMap_[connName] = conn;
    
    LOG_INFO("newConnection  name = %s , fd = %d , runInTid:%d", connName.c_str(),sockfd ,ioloop->getTid());

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWritecomplateCallback(writeCompleteCallback_);
    // conn->setCloseCallback(connectionCallback_);//flag
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, _1));
    
    ioloop->runInLoop(
        std::bind(&TcpConnection::connnectEstablished, conn));
    

}
void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    LOG_INFO("removeconnecion %s",conn->name().c_str());
    connecttionMap_.erase(conn->name()) ;
    EventLoop* ioloop =  conn->getLoop();
    ioloop->queueInLoop(std::bind(&TcpConnection::connectDestory, conn));
    //flag   runinloop ?? queueinloop
}





//tcpnewconnection
//选择subloop
//封装connfd -》 chennel 分发给subloop




//eventloop ->  poller  channels 
//poll()
//channel -> handleevent
//poller -> epoll 

//thread

//eventloopthread

//eventloopthreapool