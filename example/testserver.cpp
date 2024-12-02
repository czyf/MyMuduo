#include "../TcpServer.h"
#include "../InetAddress.h"
#include "../EventLoop.h"
#include "../Logger.h"

#include <iostream>
#include <string>

class EchoServer{
public:
    EchoServer(EventLoop *loop, const InetAddress &addr, const std::string &name)
                : loop_(loop)
                , server_(loop, addr, name)
    {
        // 注册回调
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1)
        );
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, std::placeholders::_1, 
                std::placeholders::_2, std::placeholders::_3)
        );
        // 设置合适的loop线程数量
        server_.setThreadNum(3);
    }
    void start() {
        server_.start();
    }

private:
    // 连接建立或者断开的回调
    void onConnection(const TcpConnectionPtr &conn){
        if (conn->connected()){
            LOG_INFO("conn up : %s", conn->peerAddr().toIpPort().c_str());
        }
        else{
            LOG_INFO("conn down : %s", conn->peerAddr().toIpPort().c_str());
        }
    }

    // 可读写事件回调
    void onMessage(const TcpConnectionPtr &conn, 
                Buffer *buf,
                Timestamp time)
    {
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();  // 关闭写端 低层响应EPOLLHUP事件，执行closeCallback_回调
    }

    EventLoop *loop_;
    TcpServer server_;

};


int main(){
    EventLoop loop;
    InetAddress addr(12306);
    EchoServer server(&loop, addr, "EchoServer-01");
    server.start();
    loop.loop();  // 启动mainloop的低层Poller

    return 0;
}