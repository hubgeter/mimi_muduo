#include "InetAddress.h"

#include <string.h>
#include <arpa/inet.h>

InetAddress::InetAddress(uint16_t port,std::string ip ){
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    // in_addr_t inet_addr(const char*strptr);
    // 点分十进制字符串表示的IPv4地址转化为用网络字节序整数表示的IPv4地址
}

std::string InetAddress::toIp() const {

    char buf[64]={0};
    ::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);
    return buf;
}
// std::string 
uint16_t InetAddress::toPort() const{

    return ntohs(addr_.sin_port);
}
std::string InetAddress::toIpPort() const{
    char buf[64]={0};
    ::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);
    size_t end = strlen(buf);
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf+end,":%u",port);
    return buf;
}