#include"Logger.h"
#include"Timestamp.h"

#include<string>
#include<iostream>

// 获取日志实例对象
Logger& Logger::getInstance(){
    static Logger logger;
    return logger;
}
// 设置日志级别
void Logger::setLevel(int level){
    loglevel_ = level;
}

// 写日志
void Logger::log(const std::string &msg){
    switch (loglevel_)
    {
    case INFO:
        std::cout << "[INFO]  ";
        break;
    case ERROR:
        std::cout << "[ERROR]  ";
        break;
    case FATAL:
        std::cout << "[FATAL]  ";
        break;
    case DEBUG:
        std::cout << "[DEBUG]  ";
        break;
    default:
        break;
    }
    std::cout << Timestamp::now().toString() << " : " << msg << std::endl;
}