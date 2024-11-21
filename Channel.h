#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <functional>
#include <memory>

class EventLoop;
class Timestamp;
/*
* channel理解为通道，封装了socket和其感兴趣的event，如EPOLLIN，EPOLLOUT
* 还绑定了poller返回的具体事件
*/ 
class Channel : noncopyable{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // fd得到Poller通知以后，处理事件的，调用相应的回调方法
    void handleEvent(Timestamp receiveTime);
    // 设置回调函数操作
    void setReadCallback(ReadEventCallback cb) { this->readCallback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { this->writeCallback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { this->closeCallback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { this->errorCallback_ = std::move(cb); }

    // 防止channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { this->revents_ = revt; }
    

    // 设置fd相应的事件
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    // 返回fd当前的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() const { return index_; }
    void set_index(int idx) { this->index_ = idx; }

    // one loop per thread
    EventLoop* ownerLoop() const { return loop_; }
    void remove();

private:

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;  // 事件循环
    const int fd_;    //  fd, Poller监听的对象
    int events_;       // 注册fd感兴趣的事件
    int revents_;      // Poller返回的具体发生的事件
    int index_;       // 

    std::weak_ptr<void> tie_;
    bool tied_;

    // 因为channel通道里面能够获知fd最终发生的具体的事件revents，所以他负责调用具体的事件回调操作
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;    
};