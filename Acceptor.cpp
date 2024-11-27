#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

static int createNonblocking(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
    if (sockfd < 0){
        LOG_FATAL("%s:%s:%d listen socket create err:%d \n", __FILE__,
         __FUNCTION__, __LINE__, errno);
    }
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort)
    : loop_(loop) 
    , acceptSocket_(createNonblocking())  // socket
    , acceptChannel_(loop, acceptSocket_.fd())
    , listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr);  // bind
    // TcpServer::start() Acceptor.listen 有新用户的连接，要执行一个回调（connfd =》channel => subloop)
    // baseLoop => acceptChannel_(listenfd) =>
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}
Acceptor::~Acceptor(){
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen(){
    listenning_ = true;
    acceptSocket_.listen();  // listen
    acceptChannel_.enableReading();   // acceptChannel_ => Poller
}
// listenfd有时间发生了，就是有新用户连接了，
void Acceptor::handleRead(){
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0){
        if (newConnectionCallback_){
            newConnectionCallback_(connfd, peerAddr);  // 轮询找到subloop， 唤醒、分发当前新客户端的Channel
        }
        else{
            ::close(connfd);
        }
    }
    else{
        LOG_FATAL("%s:%s:%d accept err:%d \n", __FILE__,
            __FUNCTION__, __LINE__, errno);
        
        if (errno == EMFILE){
            LOG_FATAL("%s:%s:%d socket reach limit \n", __FILE__,
                __FUNCTION__, __LINE__);
        }
    }
}