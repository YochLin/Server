#include "server.h"

Server::Server(int port, int thread_num) : thread_(new ThreadPool(thread_num)), 
                                           isClose_(false),
                                           epoller_(new Epoller())
{
    if(!InitStocket()) 
        isClose_ = true;
}

Server::~Server()
{
    isClose_ = true;
}

void Server::Start()
{
    int timeMS = -1;
    while(!isClose_) {
        int eventCnt = epoller_->Wait(timeMS);
        for(int i = 0; i < eventCnt; i++) {
            int fd = epoller_->GetEventFd(i);
        }
    }
}

void Server::InitEventMode(int trigMode)
{
    listenEvent_ = EPOLLRDHUP;
    connEvent_ = EPOLLONESHOT | EPOLLRDHUP;
    switch (trigMode)
    {
    case 0:
        break;
    case 1:
        connEvent_ |= EPOLLET;
        break;
    case 2:
        listenEvent_ |= EPOLLET;
        break;
    default:
        break;
    }
}

bool Server::InitStocket()
{
    int ret;
    struct sockaddr_in addr;
    if(port_ > 65535 || port_ < 1024){
        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);

    struct linger optLinger = {0};
    if(openLinger_){
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_ < 0){
        return false;
    }

    ret = setsockopt(sockfd_, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if(ret < 0){
        close(sockfd_);
        return false;
    }

    int optval = 1;
    ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret == -1){
        close(sockfd_);
        return false;
    }

    ret = bind(sockfd_, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0){
        return false;
    }

    ret = listen(sockfd_, 6);
    if(ret < 0){
        close(sockfd_);
        return false;
    }

    ret = epoller_->AddFd(sockfd_, listenEvent_ | EPOLLIN);
    if(ret == 0) {
        close(sockfd_);
        return false;
    }

    SetNonblock(sockfd_);
    return true;
}

int Server::SetNonblock(int sockfd)
{
    assert(sockfd > 0);
    return fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
}

void Server::DealWrite()
{

}

void Server::DealRead()
{
    
}

void Server::DealListen()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = accept(sockfd_, (struct sockaddr*)&addr, &len);
    if(fd <= 0) return;
    
    AddClient(fd, addr);
}