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
#include "timer.h"

class Server
{
public:
    explicit Server(int port, int trigMode, int thread_num, int timeout);
    ~Server();

    void Start();

private:

    bool InitStocket();
    void InitEventMode(int trigMode);
    void AddClient(int fd, sockaddr_in addr);

    void DealListen();
    void DealWrite(HttpConn* client);
    void DealRead(HttpConn* client);    

    void ExtentTime(HttpConn* client);

    void CloseConn(HttpConn* client);

    static int SetNonblock(int sockfd);

private:

    void OnWrite(HttpConn* client);
    void OnRead(HttpConn* client);
    void OnProcess(HttpConn* client);


    int port_;
    int sockfd_;
    bool openLinger_;
    int timeoutMS_;
    bool isClose_;
    char* srcDir_;
    
    uint32_t listenEvent_;
    uint32_t connEvent_;

    std::unique_ptr<Timer> timer_;
    std::unique_ptr<Epoller> epoller_;
    std::unique_ptr<ThreadPool> thread_;
    std::unordered_map<int, HttpConn> users_;
};

#endif