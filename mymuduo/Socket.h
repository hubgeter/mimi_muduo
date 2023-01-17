#pragma once 

#include"NonCopyable.h"

#include <unistd.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

class InetAddress;

// 获取sockfd对应的本端socket地址
struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);

//封装 socketfd 操作sockfd 
class Socket:NonCopyable
{
public:
    explicit Socket(int sockfd):sockfd_(sockfd){}
    ~Socket() { ::close(sockfd_); } //flag

    int fd()const {return sockfd_;}
    void bindAddress(const InetAddress&localaddr);
    void listen();
    int accept(InetAddress*peeraddr);

    void shudownWrite();

    void setTcpNoDelay(bool on);

    //socket选项SO_REUSEADDR来强制使用被处于TIME_WAIT状态的连接占用的socket地址
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;

};

