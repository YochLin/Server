#include "httpconn.h"

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