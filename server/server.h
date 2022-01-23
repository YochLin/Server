#ifndef _SERVER_H_
#define _SERVER_H_

#include <unistd.h>         // close() function
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unordered_map>

#include "eopller.h"
#include "threadpool.h"
#include "httpconn.h"

class Server
{
public:
    explicit Server(int port, int thread_num);
    ~Server();

    void Start();

private:

    bool InitStocket();
    void InitEventMode(int trigMode);
    void AddClient(int fd, sockaddr_in addr);

    void DealListen();
    void DealWrite();
    void DealRead();    

    static int SetNonblock(int sockfd);

private:

    int port_;
    int sockfd_;
    bool openLinger_;
    int timeoutMS_;
    bool isClose_;
    
    uint32_t listenEvent_;
    uint32_t connEvent_;

    std::unique_ptr<Epoller> epoller_;
    std::unique_ptr<ThreadPool> thread_;
    std::unordered_map<int, HttpConn> users_;
};

#endif