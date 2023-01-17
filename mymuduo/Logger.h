#pragma once

#include "TimeStamp.h"
#include "NonCopyable.h"

#include <cstring>
#include <string>

#define MAX_LOG_SIZE  1024
#define EXIT_STATE  -1 

#define ENABLE_LOG  0 
#define ENABLE_DEBUG  0

/*
    INFO 
    ERROR 
    FATAL
    DEBUG
*/
enum LogLevel{
    INFO,//提示
    ERROR,//错误
    FATAL,//致命错误
    DEBUG//调试 
};

// LOG_INFO(%s %s ...)
#define LOG_INFO(logmsgformat,...)\
    if (ENABLE_LOG){\
    do{\
        char buf[MAX_LOG_SIZE];\
        snprintf(buf,MAX_LOG_SIZE,"[INFO]  (%s,%s:%d) %s : ",__FILE__,__func__,__LINE__,TimeStamp::now().toString().c_str());\
        size_t _sz = strlen(buf);\
        snprintf(buf+_sz,MAX_LOG_SIZE-_sz,logmsgformat,##__VA_ARGS__ );\
        printf("%s\n",buf);\
    }while (0);}

// Logger& logger = Logger::getinstance();\   
// Logger.setLogLevel(INFO);
// logger->log(buf);

#define LOG_ERROR(logmsgformat,...)\
    if (ENABLE_LOG){\
    do{\
        char buf[MAX_LOG_SIZE];\
        snprintf(buf,MAX_LOG_SIZE,"[ERROR] (%s,%s:%d) %s : ",__FILE__,__func__,__LINE__,TimeStamp::now().toString().c_str());\
        size_t _sz = strlen(buf);\
        snprintf(buf+_sz,MAX_LOG_SIZE-_sz,logmsgformat,##__VA_ARGS__);\
        printf("%s\n",buf);\
    }while (0);}

#define LOG_FATAL(logmsgformat,...)\
    if (ENABLE_LOG){\
    do{\
        char buf[MAX_LOG_SIZE];\
        snprintf(buf,MAX_LOG_SIZE,"[FATAL] (%s,%s:%d) %s : ",__FILE__,__func__,__LINE__,TimeStamp::now().toString().c_str());\
        size_t _sz = strlen(buf);\
        snprintf(buf+_sz,MAX_LOG_SIZE-_sz,logmsgformat,##__VA_ARGS__);\
        printf("%s\n",buf);\
        exit(EXIT_STATE);\
    }while (0);}

#define LOG_DEBUG(logmsgformat,...)\
    if (ENABLE_LOG && ENABLE_DEBUG){\
    do{\
        char buf[MAX_LOG_SIZE];\
        snprintf(buf,MAX_LOG_SIZE,"[DEBUG] (%s,%s:%d) %s : ",__FILE__,__func__,__LINE__,TimeStamp::now().toString().c_str());\
        size_t _sz = strlen(buf);\
        snprintf(buf+_sz,MAX_LOG_SIZE-_sz,logmsgformat,##__VA_ARGS__);\
        printf("%s\n",buf);\
    }while (0);}



/*
日志类
*/
// class Logger:noncopyable{
// public:
//     //获取日志唯一的实例对象
//     static Logger& getinstance();
//     //设置日志级别
//     void setLogLevel(int );
    
//     //写日志
//     // void log(std::string);
//     void log(const char* msg);
// private:
//     int loglevel_;
//     Logger(){};
//     Logger(const Logger&){};
// };

