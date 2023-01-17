#pragma once 
#include<vector>
#include<string>
#include<sys/uio.h>
#include<errno.h>

#define KCheapPreend   8
#define KInitalSize  1024-8

class Buffer
{
public:

    explicit Buffer(size_t initnum = KInitalSize, size_t prenum = KCheapPreend)
        : buffer_(prenum + initnum),preIndex_(prenum) ,readerIndex_(prenum), writerIndex_(prenum)
    {
    }
    //可读数据长度
    size_t readableBytes() const{
        return writerIndex_ - readerIndex_;
    }
    //可写数据长度
    size_t writerableBytes() const{
        return buffer_.size() - writerIndex_;
    }
    //往前添加的数量
    size_t prependableBytes()const{
        return readerIndex_;
    }
    const char* peek() const{
        return begin()+ readerIndex_;
    }


    void retrieve(size_t len )
    {
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }else 
        {
            retrieveAll();
        }
    }    

    void retrieveAll(){
        readerIndex_ = writerIndex_ = preIndex_;
    }

    
    //把数据都出来转成string 
    std::string retrieveAllString()
    {
        return retrieveAsString(readableBytes());
    }
    std::string retrieveAsString(size_t len )
    {
        std::string result(peek(),len);
        retrieve(len);
        return result;
    }


    void ensuerWriteableBytes(size_t len )
    {
        if (writerableBytes() < len )
        {
            makeSpace(len);
        } 
    }
    char *beginWrite()
    {
        return begin()+ writerIndex_;
    }

    void append(const char*data,size_t size)
    {
        ensuerWriteableBytes(size);
        std::copy(data,data+size,begin());
        writerIndex_+=size;
    }

    //从fd上读取数据 LT 模式
    ssize_t readFd(int fd,int* saveError )
    {
        char extrabuf[65536];
        //栈上的空间
        struct iovec vec[2];  //readv 缓冲区不需要在一块
        const size_t writeable = writerableBytes();
        vec[0].iov_base = beginWrite();
        vec[0].iov_len = writeable;
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof extrabuf;
        size_t iovcnt = (writeable < sizeof extrabuf)?2:1;
        //写道不连续的空间 
        //栈上申请内存快
        const ssize_t n = readv(fd,vec,iovcnt); 
        if (n<0)
        {
            *saveError = errno;
        }
        else if(n <= writeable)
        {
            writerIndex_ += n;
        }else{
            writerIndex_=buffer_.size();
            append(extrabuf,n-writeable);
        }
        return n;
    }


private:
    char *begin(){
        return buffer_.data();
    }
    const char *begin() const {
        return buffer_.data();
    }
    // 扩容
    void makeSpace(size_t len )  
    {
        if (writerableBytes() + prependableBytes() < len + preIndex_)
        {   
            buffer_.resize(len+writerIndex_);
        }
        else
        {
            std::copy(begin() + readerIndex_,begin() + writerIndex_,begin() + preIndex_);
            readerIndex_ = preIndex_;
            writerIndex_ = readerIndex_+readableBytes();

        }

    }
    

    std::vector<char> buffer_;
    size_t preIndex_;//用于添加头部
    size_t readerIndex_;
    size_t writerIndex_;

};