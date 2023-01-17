
#include "NonCopyable.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include<vector>
#include<memory>

class EventLoopThreadPoll: NonCopyable
{
public:
    using ThreadInitCallbal = std::function<void(EventLoop*)>;

    EventLoopThreadPoll(EventLoop* baseloop,int numThread = 0);
    ~EventLoopThreadPoll();


    void start(const ThreadInitCallbal& );

    void setThreadNum(int numThreads){numThread_ = numThreads;}

    //如果工作在多线程，baseloop默认以轮询的方式分配channel 给subloop 得到轮询到的subloop
    EventLoop* getNextLoop();

    //得到所有的工作线程
    std::vector<EventLoop*>getAllLoops();

    bool started() const {return started_;}
    

private:
    
    EventLoop *baseLoop_;//mainloop 
    bool started_;
    int numThread_;// 其实是subeactor 数量
    int next_;//轮询到的下一个subloop 
    std::vector<std::unique_ptr<EventLoopThread>> thread_;
    std::vector<EventLoop*> loops_;
};