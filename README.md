# mimi_muduo

## 项目简介

**基于C++11**实现muduo网络库的核心功能：**TcpServer**，在使用方面与muduo基本一致。

多线程模型与muduo网络库一致。

使用C++11的新语法：智能指针、function函数对象、lambda表达式、bind绑定器、并发库（thread , atomic,mutex,condition_variable ）等。

整个项目采用**cmake**来进行编译。


## 使用

项目附带了一个示例：echo回显服务器

```shell
cd mymuduo
./autobuild.sh
cd example
make
./echo
```


## 待改进的部分

1. 只实现了epoll这一个IO-Multiplexing,没有实现poll/select。

2. 日志模块中使用printf进行输出，printf线程安全，但效率不高。

3. 只实现了TcpServer部分，暂时没有做TcpClient和TimerQueue部分。