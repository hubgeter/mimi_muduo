#pragma once 

#include "NonCopyable.h"

#include<thread>
#include<memory>
#include<unistd.h>
#include<functional>
#include<string>
#include<atomic>

class Thread:NonCopyable
{
public:
    using ThreadFunc = std::function<void()>;
    //绑定器 + 函数对象 

    explicit Thread(ThreadFunc );
    ~Thread();

    void start();
    void join();

    bool started() const {return started_; }
    pid_t tod() const {return tid_;}

    static int getThreadNum() {return numThread_.load();}
private:
    bool started_;
    bool joined_;
    // std::thread thread_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    
    ThreadFunc func_;
    std::string name_;//线程名字

    static std::atomic<int> numThread_; 
};