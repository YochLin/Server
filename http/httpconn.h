#ifndef _HTTP_CONN_H_
#define _HTTP_CONN_H_

#include <cstdint>
#include <mutex>
#include <arpa/inet.h>

#include "httprequest.h"
#include "httpresponse.h"
#include "buffer.h"
class HttpConn
{
public:
    HttpConn();
    ~HttpConn();

    void Init(int32_t sockfd, const sockaddr_in& addr);

    ssize_t Read(int* saveError);
    ssize_t Write(int* saveError);
    int ToWriteBytes();

    int32_t GetFd() const;
    sockaddr_in GetAddr() const;
    const char* GetIp() const;
    uint16_t GetPort() const;

    bool IsKeepAlive();

    bool Process();

    void Close();

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;

    Buffer readBuff_;
    Buffer writeBuff_;

private:
    int32_t fd_;
    struct sockaddr_in addr_;

    bool isClose_;

    int iov_cnt_;
    struct iovec iov_[2];

    HttpRequest request_;
    HttpResponse response_;
};

#endif 