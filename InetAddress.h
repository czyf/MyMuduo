#pragma once

#include<netinet/in.h>
#include<string>

// 封装socket地址类型
class InetAddress{
public:
    explicit InetAddress(uint16_t port, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr) : 
        addr_(addr)
        {}
    uint16_t toPort() const;
    std::string toIp() const;
    std::string toIpPort() const;
    /*const sockaddr_in* 表示返回的指针所指向的 sockaddr_in 结构体是只读的，无法通过该指针修改 sockaddr_in 的内容。
    这种设计通常用在返回类中私有成员的指针时，以保护数据的完整性。
    */ 
    const sockaddr_in* getSockAddr() const { return &addr_; }
private:
    sockaddr_in addr_;

};