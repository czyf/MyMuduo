#pragma once
/*
* noncopyable被继承以后，派生类无法进行拷贝构造和赋值操作，可以正常的构造或析构
*/
class noncopyable{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};