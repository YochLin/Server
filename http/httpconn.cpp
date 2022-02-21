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

ssize_t HttpConn::Read(int* saveError)
{
    ssize_t len = -1;
    do {
        len = readBuff_.Readfd(fd_, saveError);
        printf("%s, %d, %d\n", __func__, __LINE__, len);
        if(len <= 0)
            break;
    } while(isET);
    return len;
}

ssize_t HttpConn::Write(int* saveErrno)
{
    ssize_t len = -1;
    // do {
    //     len = writeBuff_.Writefd(fd_);
    //     printf("%s, %d, %d, %s\n", __func__, __LINE__, len, writeBuff_.GetData());
    //     if(len <= 0)
    //         break;
    // } while(isET);
    do {
        len = writev(fd_, iov_, iov_cnt_);
        if(len <= 0) {
            *saveErrno = errno;
            break;
        }
        if(ToWriteBytes() == 0)
            break;
        else if(static_cast<size_t>(len) > iov_[0].iov_len) {
            iov_[1].iov_base = (uint8_t*)iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            if(iov_[0].iov_len) {
                writeBuff_.RetrieveAll();
                iov_[0].iov_len = 0;
            }
        }
        else {
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            writeBuff_.Retrieve(len);
        }
    } while(isET || ToWriteBytes() > 10240);
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
        return false;
    }
    else if(request_.Parse(readBuff_)) {
        response_.Init(srcDir, request_.GetPath(), request_.IsKeepAlive(), 200);
    }
    else {
        response_.Init(srcDir, request_.GetPath(), false, 400);
    }
    response_.MakeResponse(writeBuff_);

    // response head
    iov_[0].iov_base = const_cast<char*>(writeBuff_.Peek());
    iov_[0].iov_len = writeBuff_.ReadableBytes();
    iov_cnt_ = 1;

    // response content 
    if(response_.FileLength() > 0) {
        iov_[1].iov_base = response_.File();
        iov_[1].iov_len = response_.FileLength();
        iov_cnt_ = 2;
    }
    printf("%s, File Length: %d, iov: %d to %d\n", __func__, response_.FileLength(), iov_cnt_, ToWriteBytes());
    return true;
}