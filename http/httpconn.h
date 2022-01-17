#ifndef _HTTP_CONN_H_
#define _HTTP_CONN_H_

#include <cstdint>
#include <arpa/inet.h>

class HttpConn
{
public:
    HttpConn();
    ~HttpConn();

    void Init(int32_t sockfd, const sockaddr_in& addr);

    ssize_t Read();
    ssize_t Write();

    int32_t GetFd() const;

    sockaddr_in GetAddr() const;

    const char* GetIp() const;

    uint16_t GetPort() const;

private:
    int32_t fd_;
    struct sockaddr_in addr_;

    bool isClose_;

};

#endif 