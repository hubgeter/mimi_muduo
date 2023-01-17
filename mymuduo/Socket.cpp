#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"

#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/tcp.h>

void Socket::bindAddress(const InetAddress&localaddr){
    if(0 != bind(sockfd_, static_cast<sockaddr*>((void *)&localaddr.getSockaddr()),sizeof(sockaddr_in)))
    {
        LOG_FATAL("bind fatal:%s", localaddr.toIpPort().c_str());
    }
}
struct sockaddr_in getLocalAddr(int sockfd)
{
    struct sockaddr_in localaddr;
    bzero(&localaddr,sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr); 
    if (::getsockname(sockfd,static_cast<sockaddr*>((void*)&localaddr),&addrlen) < 0)
    { 

    }
    return localaddr;
}
struct sockaddr_in getPeerAddr(int sockfd)
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr,sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr); 
    if (::getpeername(sockfd,static_cast<sockaddr*>((void*)&peeraddr),&addrlen) < 0 )
    {
    
    }
    return peeraddr;
}


void Socket::listen(){
    if (0 != ::listen(sockfd_,1024))
    {
        
    }
}
#include<strings.h>
int Socket::accept(InetAddress*peeraddr)
{
    sockaddr_in addr;
    socklen_t len = sizeof addr;
    bzero(&addr,sizeof addr);
    int connfd = ::accept(sockfd_,(sockaddr*)&addr,&len);
    if (connfd >= 0)
    {
        peeraddr->setSockAddr(addr);

    } else {
        LOG_ERROR("connfd error");
      
    }
    return connfd;

}
//关闭写端 
void Socket::shudownWrite()
{
    if(::shutdown(sockfd_,SHUT_WR) < 0)
    {

    }
}

void Socket::setTcpNoDelay(bool on)
{

    int optval = on?1:0;
    ::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof optval);
}
void Socket::setReuseAddr(bool on)
{   
    int optval = on?1:0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
}
void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
}
void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
    //TCP_KEEPINTVL//flag
}