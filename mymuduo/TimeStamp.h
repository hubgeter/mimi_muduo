#pragma once 

#include<string>
/*
时间类
*/

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t);

    static TimeStamp now();
    std::string toString() const;

private:
    int64_t microSecodsSinceEpoch_;
};