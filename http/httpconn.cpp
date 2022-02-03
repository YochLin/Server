#include "httpconn.h"

bool HttpConn::isET;
const char* HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;

HttpConn::HttpConn()
{
    fd_ = -1;
    addr_ = {0};
    isClose_ = false;
}

HttpConn::~HttpConn()
{

}

void HttpConn::Init(int32_t sockfd, const sockaddr_in& addr)
{
    fd_ = sockfd;
    addr_ = addr;
    isClose_ = false;
    writeBuff_.RetrieveAll();
    readBuff_.RetrieveAll();
    userCount++;   
}

int32_t HttpConn::GetFd() const
{
    return fd_;
}

sockaddr_in HttpConn::GetAddr() const
{   
    return addr_;
}

const char* HttpConn::GetIp() const
{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t HttpConn::GetPort() const
{
    return addr_.sin_port;
}

void HttpConn::Close()
{
    if(isClose_ == false) {
        isClose_ = true;
        userCount --;
        close(fd_);
    }
}

ssize_t HttpConn::Read()
{
    ssize_t len = -1;
    do {
        len = readBuff_.Readfd(fd_);
        printf("%s, %d, %d\n", __func__, __LINE__, len);
        if(len <= 0)
            break;
    } while(isET);
    return len;
}

ssize_t HttpConn::Write()
{
    ssize_t len = -1;
    do {
        len = writeBuff_.Writefd(fd_);
        printf("%s, %d, %d\n", __func__, __LINE__, len);
        if(len <= 0)
            break;
    } while(isET);
    return len;
}

int HttpConn::ToWriteBytes()
{
    return iov_[0].iov_len + iov_[1].iov_len;
}

bool HttpConn::IsKeepAlive()
{
    return request_.IsKeepAlive();
}

bool HttpConn::Process()
{
    request_.Init();
    
    if(readBuff_.ReadableBytes() <= 0) {
        printf("%s, %d\n", __func__, __LINE__);
        return false;
    }
    else if(request_.Parse(readBuff_)) {
        printf("%s, %d\n", __func__, __LINE__);
        response_.Init(srcDir, request_.GetPath(), request_.IsKeepAlive(), 200);
    }
    else {
        printf("%s, %d\n", __func__, __LINE__);
        response_.Init(srcDir, request_.GetPath(), false, 400);
    }
    response_.MakeResponse(writeBuff_);
    return true;
}