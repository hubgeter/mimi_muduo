#include <mymuduo/EpollPoller.h>
#include <mymuduo/TcpServer.h>
#include <mymuduo/InetAddress.h>
#include <mymuduo/Buffer.h>
#include <iostream>

using namespace std;
using namespace placeholders;

void Connection(const TcpConnectionPtr &conn)
{
    if (conn->isConnected())
    {
        std::cout << "new connection" << std::endl;
    }
    else 
    {
        std::cout << "disconnect" << std::endl;
    }
}

void RecvData(const TcpConnectionPtr & conn , Buffer * buffer, TimeStamp recvtime)
{
    std::string recv = move(buffer->retrieveAllString());
    cout << recvtime.toString() << " : " << recv << endl;
    conn->send(recv);
}
void WriteComplate(const TcpConnectionPtr & conn)
{
    cout << conn->name() << ":WriteComplate!" << endl;
}
int main()
{
    InetAddress local(9999);
    EventLoop loop;
    TcpServer server(&loop, local);
    server.setThreadnum(2);
    server.setConnectionCallback(bind(Connection,_1));
    server.setMessageCallback(bind(RecvData, _1, _2, _3));
    server.setWriteComplateCallback(bind(WriteComplate, _1));


    server.start();
    loop.loop();
    

    return 0;
}
