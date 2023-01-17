// #include "Logger.h"
// #include "TimeStamp.h"
//
// #include <iostream>
//
// Logger& Logger::getinstance(){
// pthread_once 
// }
// void Logger::setLogLevel(int level){
//     loglevel_ = level;
// }
// //[级别] time : msg
// // void Logger::log(std::string msg)
// void Logger::log(const char* msg)
// {
//     switch (loglevel_)
//     {
//         case INFO:
//             std::cout<<"[INFO]";
//             break;
//         case ERROR:
//             std::cout<<"[ERROR]";
//             break;
//         case FATAL:
//             std::cout<<"[FATAL]";
//             break;
//         case DEBUG:
//             std::cout<<"[DEBUG]";
//             break;
//         default:
//             break;
//     }
//     // std::cout<<"time" <<" : ";
//     std::cout<<TimeStamp::now().toString()<<" : ";
//     std::cout<<msg<<std::endl;
// }


// g++ Logger.cpp TimeStamp.cpp -o tmp && ./tmp 
// #include "Logger.h"
// int main()
// {
//     LOG_INFO("hello ");
//     LOG_INFO("hello world %d", 1);
//     LOG_DEBUG("hello world %d" ,1);
//     LOG_ERROR("hello world %d" ,1);
//     LOG_FATAL("hello world %d" ,1);

// LOG_INFO("hello world %d" ,2);
//     LOG_DEBUG("hello world %d" ,2);
//     LOG_ERROR("hello world %d" ,1);
//     LOG_FATAL("hello world %d" ,1);
// }