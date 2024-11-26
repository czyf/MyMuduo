#include "EventLoopThread.h"
#include "EventLoop.h"

#include <memory>


EventLoopThread::EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), 
    const std::string &name = std::string())
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc, this), name)
    , mutex_()
    , cond_()
    , callback_(cb)
{}

EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    if (loop_ != nullptr){
        loop_->quit();
        thread_.join();  // 等待子线程结束
    }
}
EventLoop* EventLoopThread::startLoop(){
    thread_.start();  // 启动低层的新线程

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_ == nullptr){
            cond_.wait(lock); 
        }
        loop = loop_;
    }
    return loop;
}

// 下面这个方法是在单独的新县城里面运行的
void EventLoopThread::threadFunc(){
    
    EventLoop  loop; // 创建一个独立的EventLoop，和上面线程是一一对应的 onr loop per thread

    if (callback_){
        callback_(&loop);
    }
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();  // EventLoop loop => Poller poll
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;

}