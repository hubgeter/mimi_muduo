#pragma once 

#include "EventLoop.h"
#include "NonCopyable.h"
#include "TimeStamp.h"

#include<functional>
#include<memory>

class EventLoop;

/*

    封装了socketfd 和其感兴趣的event(EPOLLIN .. )
    绑定了poller返回的具体条件
    //channel 跟loop 通信 靠请求 

    文件fd + 事件 

    感兴趣的事件 ， 回调函数 
*/
class Channel : NonCopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(TimeStamp)>;
    

    Channel(EventLoop * ,int fd);
    ~Channel();

    //fd 得到poller通知以后，处理事件 调用相应的回调方法 
    void handleevent(TimeStamp );
    
    
    //设置回调函数对象
    void setReadCallback(ReadEventCallback cb){ readcallback_ = std::move(cb);}
    void setWriteCallback(EventCallback cb){ writecallback_ = std::move(cb);}
    void setCloseCallback(EventCallback cb){ closecallback_ = std::move(cb);}
    void setErrorCallback(EventCallback cb) { errorcallback_ = std::move(cb); }


    //xx 防止当channel 被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd()const {return fd_;}
    int events()const {return events_;}
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() const {return events_ == KnoneEvent;}


    //开始/关闭 读/写监听事件 操作events_ 
    void enableReading(){ events_ |= KreadEvent; update();}
    void disableReading(){ events_ &= ~KreadEvent;update();}
    void enableWriteing(){ events_ |= KwriteEvent; update();}
    void disableWriteing(){events_ &= ~KwriteEvent;update();}
    void disableall(){events_ = KnoneEvent;update();}


    //返回fd当前监听的事件状态
    bool isNoneevent() const { return events_ == KnoneEvent; }
    bool isWriting() const{ return events_ & KwriteEvent; }
    bool isReading() const{ return events_ & KreadEvent; }

 
    int index() const {return index_;}
    void set_index(int idx){index_ = idx;}

    //所在的eventloop
    EventLoop* ownerLoop(){return loop_;}

    //在channel 所属的eventloop中，删除当前channel
    void remove();

    
private:
    /*
    update() -> loop.update() -> epoll_ctl 修改，更新事件
    当改变channel 所表示fd的events事件后，update 负责在poller里面改fd相应的事件
    通过channel所属的eventloop，调用poller对应的方法，更新fd的events事件
    */
    void update();

    /*
    根据poller通知的channel发生的具体事件-->revent_ 负责调用具体的回调操作
    */ 
    void handleEventWithGuard(TimeStamp);

    //所监听事件标识 :无事件 读 写 
    static const int KnoneEvent;
    static const int KreadEvent;
    static const int KwriteEvent;

    EventLoop* loop_;
    const int fd_;//fd ，poll监听的对象 
    int events_;//注册fd感兴趣的事件
    int revents_;//poller返回的具体发生的事件
    int index_;//在epool上的状态 

    std::weak_ptr<void> tie_;
    bool tied_;


    //因为channel里面能获知fd最终发生的事件revents,所以他负责调用具体事件的回调操作
    ReadEventCallback readcallback_;
    EventCallback writecallback_;
    EventCallback closecallback_;
    EventCallback errorcallback_;
};