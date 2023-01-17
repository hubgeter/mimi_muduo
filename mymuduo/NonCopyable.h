#pragma once 

/*
继承类可以正常构造析构，但无法拷贝构造与赋值
*/
class NonCopyable
{

public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

    
