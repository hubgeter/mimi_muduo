#include"Acceptor.h"
 


static int create_listen_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

    return sockfd;
}
Acceptor::Acceptor(EventLoop *loop, const InetAddress & listenaddr,bool reuseport )
       : loop_(loop)
       , acceptSocket_(create_listen_socket())
       , acceptChannel_(loop, acceptSocket_.fd())
       , listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(listenaddr);
    
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleNewConnection,this,std::placeholders::_1));

    // acceptChannel_.setReadCallback(
    //     std::bind(&Acceptor::handleNewConnection,this)
    // );//flag
}


Acceptor::~Acceptor(){
    acceptChannel_.disableall();
    acceptChannel_.remove();
    ::close(acceptSocket_.fd());
}

void Acceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleNewConnection(TimeStamp newConnectionTime)
{
    InetAddress peerAddr(0);
    int connfd = acceptSocket_.accept(&peerAddr);

    LOG_INFO("%s", peerAddr.toIpPort().c_str());
    
    if (connfd >= 0)
    {
        LOG_INFO("new connecting , time = %s",newConnectionTime.toString().c_str());
        
        if (newConnectionCallback_)
        {
            newConnectionCallback_(connfd, peerAddr);
        } else
        {
            LOG_INFO("non have newConnectionCallback_");
            ::close(connfd);
        }
    }else 
    {

        LOG_ERROR("accept_error = %d",errno);
    
    }
}

