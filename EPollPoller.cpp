#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"
#include "Timestamp.h"

#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <unistd.h>


// channel未添加道poller中
const int kNew = -1;  // channel的成员index_ = -1
// channel已添加到poller中
const int kAdded = 1;
// channel从poller中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop) 
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{
    if (epollfd_ < 0){
        LOG_FATAL("epoll_create error:%d \n", errno);
    }
}

EPollPoller::~EPollPoller(){
    ::close(epollfd_);
}

// 对应epoll_ctl
// channel update remove => EventLoop updateChannel removeChannel => Poller updateChannel
/*
* EventLoop包含了Channel List和Poller，Poller包含ChannelMap<fd,channel*>
*/
void EPollPoller::updateChannel(Channel *channel){
    const int index = channel->index();
    LOG_INFO("func=%s => fd=%d evnets=%d index=%d \n", __FUNCTION__, channel->fd(), channel->events(), index);

    if (index == kNew || index == kDeleted){
        if (index == kNew){
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);

    }
    else{  // channel已经在poller上注册过了
        int fd = channel->fd();
        if (channel->isNoneEvent()){
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else{
            update(EPOLL_CTL_MOD, channel);
        }

    }
}
// 对应epoll_ctl 从poller中删除channel
void EPollPoller::removeChannel(Channel *channel){
    int fd = channel->fd();
    channels_.erase(fd);

    LOG_INFO("func=%s => fd=%d \n", __FUNCTION__, channel->fd());

    int index = channel->index();
    if (index == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);

}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels){
    // 实际上应该用LOG_DEBUG输出日志更为合理
    LOG_INFO("func=%s => fd total count:%lu\n", __FUNCTION__, channels_.size());

    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno; 
    Timestamp now(Timestamp::now());

    if (numEvents > 0){
        LOG_INFO("%d events happend \n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        
        if (numEvents == events_.size()){
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0){
        LOG_DEBUG("%s timeout! \n", __FUNCTION__);
    }
    else{  // 发生错误
        if (saveErrno != EINTR){
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() err!");
        }
    }
    return now;
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const{
    for (int i = 0; i < numEvents; ++i){
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);   // 发生的事件
        activeChannels->push_back(channel); // EventLoop就拿到了它的poller给他返回的所有发生事件的channel列表了   
    }
}

// 更新channel通道
void EPollPoller::update(int operation, Channel *channel){
    epoll_event event;
    memset(&event, 0, sizeof(event));

    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;
    

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0){
        if (operation == EPOLL_CTL_DEL){
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        }
        else {
            LOG_FATAL("epoll_ctl add/med error:%d\n", errno);
        }
    }
}

