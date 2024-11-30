#pragma once 

#include <vector>
#include <string>
#include <algorithm>

// 网络低层的缓冲区类型定义
class Buffer{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitalSize = 1024;

    explicit Buffer(size_t initialSize = kInitalSize)
        : buffer_(kCheapPrepend + initialSize)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend)
    {}
    // 可读的长度
    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }
    // 可写的长度
    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }
    // 前缀长度
    size_t prependableBytes() const {
        return readerIndex_;
    }
    // 返回缓冲区中可读数据的起始地址
    const char* peek() const {
        return begin() + readerIndex_;
    }
    // onMessage string <- Buffer
    void retrieve(size_t len){
        if (len < readableBytes()){
            readerIndex_ += len;  // 应用只读取了可读缓冲区数据的一部分，即len长度，还剩下rederIndex_ += len -> writerIndex_ 
        }
        else{             // len = readableBytes
            retrieveAll();
        }
    }
    //
    void retrieveAll(){
        readerIndex_ = writerIndex_ = kCheapPrepend;
    }
    // 将onMessage函数上报的Buffer数据，转成string类的数据返回  retrieve:读取、检索
    std::string retrieveAllAsString(){
        return retrieveAsString(readableBytes());  // 应用可读取数据的长度
    }

    std::string retrieveAsString(size_t len){
        std::string result(peek(), len);
        retrieve(len);  // 上面一句把缓冲区中可读的数据，已经读取出来，这里要对缓冲区进行复位操作
        return result;
    }
    // buffer_.size - writerIndex_ ? len
    void ensureWriteableBytes(size_t len){
        if (writableBytes() < len){
            makeSpace(len);  // 扩容函数
        }
    }
    // 把[data, data+len]上的数据添加道writeable缓冲区当中
    void append(const char *data, size_t len){
        ensureWriteableBytes(len);
        std::copy(data, data+len, beginWrite());
        writerIndex_ += len;
    }

    char* beginWrite(){
        return begin() + writerIndex_;
    }

    const char* beginWrite() const {
        return begin() + writerIndex_;
    }
    // 从fd上读取数据
    ssize_t readFd(int fd, int* saveErrno);
private:
    char* begin(){
        return &*buffer_.begin();
    }
    const char* begin() const{
        return &*buffer_.begin();
    }

    void makeSpace(size_t len){
        if (writableBytes() + prependableBytes() < len + kCheapPrepend){
            buffer_.resize(writerIndex_ + len);
        }
        else{
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                        begin() + writerIndex_,
                        begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

    

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};