#pragma once 


#include <string>
#include <netinet/in.h>
#include <strings.h>

/*
    socket地址 ipv4地址类型
    ip + port 
*/
class InetAddress
{
public:
    explicit InetAddress(uint16_t port){
        bzero(&addr_, sizeof addr_);
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = htonl(INADDR_ANY);
        addr_.sin_port = htons(port);
    }
    
    explicit InetAddress(uint16_t port,std::string);

    explicit InetAddress(const sockaddr_in & addr)
        :addr_(addr)
    {
        
    }
    
    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;

    //得到 ipv4 socket 地址
    const sockaddr_in& getSockaddr() const 
    {
        return addr_;
    }

    //设置 ipv4 socket 地址
    void setSockAddr(const sockaddr_in &addr) 
    { 
        addr_ = addr; 
    };

private:
    sockaddr_in addr_;
};

// 两个专用socket地址结构
// sockaddr_in  IPv4
// sockaddr_in6 IPv6