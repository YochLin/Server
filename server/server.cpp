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

    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd_ < 0){
        return false;
    }

    ret = setsockopt(listenFd_, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if(ret < 0){
        close(listenFd_);
        return false;
    }

    int optval = 1;
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret == -1){
        close(listenFd_);
        return false;
    }

    ret = bind(listenFd_, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0){
        return false;
    }

    ret = listen(listenFd_, 6);
    if(ret < 0){
        close(listenFd_);
        return false;
    }

    return true;
}