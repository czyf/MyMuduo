#pragma once

#include"noncopyable.h"
#include<string>


// LOG_INFO("%d, %s", arg1, arg2)
#define LOG_INFO(logmsgFormat, ...) \
    do \
    {   \
        Logger &logger = Logger::getInstance(); \
        logger.setLevel(INFO);  \
        char buf[1024] = {0};   \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);   \
        Logger.log(buf);    \
    }while(0)

#define LOG_ERROR(logmsgFormat, ...) \
    do \
    {   \
        Logger &logger = Logger::getInstance(); \
        logger.setLevel(ERROR);  \
        char buf[1024] = {0};   \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);   \
        Logger.log(buf);    \
    }while(0)

#define LOG_FATAL(logmsgFormat, ...) \
    do \
    {   \
        Logger &logger = Logger::getInstance(); \
        logger.setLevel(FATAL);  \
        char buf[1024] = {0};   \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);   \
        Logger.log(buf);    \
    }while(0)

#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...) \
    do \
    {   \
        Logger &logger = Logger::getInstance(); \
        logger.setLevel(DEBUG);  \
        char buf[1024] = {0};   \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);   \
        Logger.log(buf);    \
    }while(0)
#else
#define LOG_DEBUG(logmsgFormat, ...)
#endif
// 日志的级别，
enum LogLevel{
    INFO,   // 普通信息
    ERROR,  // 错误信息
    FATAL,  // core信息
    DEBUG, // 调试信息
};

// 输出一个日志类
class Logger : noncopyable{
public:
    // 获取日志的唯一对象
    static Logger& getInstance();
    // 设置日志等级
    void setLevel(int level);
    // 写日志
    void log(const std::string &msg);
private:
    Logger();
    int loglevel_;
};