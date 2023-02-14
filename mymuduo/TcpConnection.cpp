#include "TcpConnection.h"
#include <netinet/tcp.h>

TcpConnection::TcpConnection(EventLoop *loop,
                             int sockfd,
                             std::string name,
                             InetAddress localaddr,
                             InetAddress peeraddr)
     : loop_(loop) // subloop 
    , name_(name)
    , socket_(new Socket(sockfd))
    , channel_(new Channel(loop, sockfd))
    , localAddr_(localaddr)
    , peerAddr_(peeraddr)
    , state_(KConnecting)
    , reading_(false)
{
    
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError ,this));

    socket_->setKeepAlive(true);
}
TcpConnection::~TcpConnection()
{
    

}

void TcpConnection::send(const std::string& message)
{
    if(state_ == KConnected)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(message);
        }else{

            //FIXME
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, move(std::string(message))));
        }
    }        
}
void TcpConnection::sendInLoop(std::string message)
{
    LOG_INFO("send fd = %d , tid= %d",channel_->fd() ,loop_->getTid() );

    size_t sz = 0;
    // channel_->isWriting() 看channel是否在poller监听 如果在监听，则还不能写
    if (!channel_->isWriting() && outbuffer_.readableBytes() == 0)
    {
        sz = ::write(channel_->fd(), message.data(), message.size());

        if (sz >= 0)
        {
            if ( sz == message.size() && writeCompleteCallback_)
                loop_->queueInLoop(std::bind(
                    writeCompleteCallback_, shared_from_this()));

            if (sz < message.size())//写了部分
            {

            }else
            {
                //写错误
            }
        }
    }

    if (sz < message.size())//现在不能写/写了部分
    {
        outbuffer_.append(message.data()+sz,message.size()-sz);
        if(!channel_->isWriting())
        {
            channel_ -> enableWriteing();//监听channel的写事件
        }
    }

}
void TcpConnection::shutdown()
{
    if (state_ == KConnected)
    {
        set_State(KDisconnected);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}
void TcpConnection::shutdownInLoop()
{
    if (!channel_-> isWriting())
    {
        socket_->shudownWrite();
    }
}


void TcpConnection::connnectEstablished() 
{
    set_State(KConnected); 
    channel_->tie(shared_from_this());
    reading_ = true;
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestory()
{

    if (state_ == KConnected)
    {
        set_State(KDisconnected);
        channel_->disableall();
        connectionCallback_(shared_from_this());
    }

    channel_->remove();
}




void TcpConnection::handleRead(TimeStamp recvtime)
{
    int saveerrno = 0;
    ssize_t n = inbuffer_.readFd(channel_->fd(), &saveerrno);
    if ( n > 0 )
    {
        messageCallback_(shared_from_this(), &inbuffer_, recvtime);
    }
    else if(n == 0)
    {
        //recv 返回0，这意味着通信对方已经关闭连接了
        handleClose();
    } 
    else 
    {
        errno = saveerrno;   
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    if (channel_->isWriting())//poll上监听channel的写事件
    {   
        size_t n = ::write(channel_->fd(),outbuffer_.peek(),outbuffer_.readableBytes());
        if (n > 0)
        {
            outbuffer_.retrieve(n);
            if (outbuffer_.readableBytes() == 0)
            {
                channel_->disableWriteing();//写完则关闭写事件的监听
                if (writeCompleteCallback_)
                {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                    // writeCompleteCallback_(shared_from_this());//flag 
                }
            }

        }
    }
}
//buffer -> writefd
//writecomplatecallback-;
void TcpConnection::handleClose()
{
    set_State(KDisconnected);

    channel_->disableall();
    
    connectionCallback_(shared_from_this());//flag
    closeCallback_(shared_from_this());
}
void TcpConnection::handleError()
{
    LOG_ERROR("errono = %d (%s)", errno,gai_strerror(errno));
    //错误码转换成其字符串形式
}

void TcpConnection::startReadInLoop()
{
    if (!reading_ ||!channel_->isReading())
    {
        channel_->enableReading();
        reading_ = true;
    }
}
void TcpConnection::stopReadInLoop()
{
    if (reading_ || channel_->isReading())
    {
        channel_->disableReading();
        reading_ = false;
    }
}