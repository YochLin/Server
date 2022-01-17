#include "server.h"

Server::Server(int port)
{

}

Server::~Server()
{

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