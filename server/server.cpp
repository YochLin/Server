#include "server.h"

Server::Server(int port, int trigMode, int thread_num, 
               int timeout) : port_(port),
                              thread_(new ThreadPool(thread_num)), 
                              isClose_(false),
                              epoller_(new Epoller()),
                              timeoutMS_(timeout),
                              timer_(new Timer())
{
    srcDir_ = getcwd(nullptr, 256);
    strncat(srcDir_, "/resources/", 16);
    HttpConn::userCount = 0;
    HttpConn::srcDir = srcDir_;

    
    InitEventMode(trigMode);
    if(!InitStocket()) {
        isClose_ = true;
        printf("True\n");
    }
        
}

Server::~Server()
{
    isClose_ = true;
    close(sockfd_);
    free(srcDir_);
}

void Server::Start()
{
    int timeMS = -1;
    while(!isClose_) {
        if(timeoutMS_ > 0)
            timeMS = timer_->GetNextTick();
        int eventCnt = epoller_->Wait(timeMS);
        for(int i = 0; i < eventCnt; i++) {
            
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvents(i);
            printf("%s, %d\n", __func__, __LINE__);
            if(fd == sockfd_) 
                DealListen();
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(users_.count(fd) > 0);
                CloseConn(&users_[fd]);
            }
            else if(events & EPOLLIN) {
                assert(users_.count(fd) > 0);
                DealRead(&users_[fd]);
            }
            else if(events & EPOLLOUT) {
                assert(users_.count(fd) > 0);
                DealWrite(&users_[fd]);
            }
            else {
                printf("Unexpected event\n");
            }
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
    case 3:
        listenEvent_ |= EPOLLET;
        connEvent_ |= EPOLLET;
        break;
    default:
        break;
    }
    HttpConn::isET = (connEvent_ & EPOLLET);
}

bool Server::InitStocket()
{
    int ret;
    struct sockaddr_in addr;
    if(port_ > 65535 || port_ < 1024){
        printf("socket wrong %d\n", __LINE__);
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
        printf("socket wrong %d\n", __LINE__);
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
        printf("socket wrong %d\n", __LINE__);
        close(sockfd_);
        return false;
    }

    ret = bind(sockfd_, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0){
        return false;
    }

    ret = listen(sockfd_, 6);
    if(ret < 0){
        printf("socket wrong %d\n", __LINE__);
        close(sockfd_);
        return false;
    }

    ret = epoller_->AddFd(sockfd_, listenEvent_ | EPOLLIN);
    if(ret == 0) {
        printf("socket wrong %d\n", __LINE__);
        close(sockfd_);
        return false;
    }

    SetNonblock(sockfd_);
    printf("%s, %d\n", __func__, __LINE__);
    return true;
}

int Server::SetNonblock(int sockfd)
{
    assert(sockfd > 0);
    return fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
}

void Server::OnWrite(HttpConn* client)
{
    int ret = -1;
    ret = client->Write();
    if(client->ToWriteBytes() == 0) {
        if(client->IsKeepAlive()) {
            OnProcess(client);
            return;
        }
    }
    else if(ret < 0) {
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
        return;
    }
    printf("%s, %d\n", __func__, __LINE__);
    CloseConn(client);
}

void Server::OnRead(HttpConn* client)
{
    int ret = -1;
    ret = client->Read();
    // if(ret < 0) {
    //     printf("Close read\n");
    //     CloseConn(client);
    //     return;
    // }
    OnProcess(client);
}

void Server::OnProcess(HttpConn* client)
{
    printf("%s, %d\n", __func__, __LINE__);
    if(client->Process())
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
    else
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLIN);
}

void Server::DealWrite(HttpConn* client)
{
    printf("%s, %d\n", __func__, __LINE__);
    ExtentTime(client);
    thread_->AddTask(std::bind(&Server::OnWrite, this, client));
}

void Server::DealRead(HttpConn* client)
{
    printf("%s, %d\n", __func__, __LINE__);
    ExtentTime(client);
    thread_->AddTask(std::bind(&Server::OnRead, this, client));
}

void Server::DealListen()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do {
        int fd = accept(sockfd_, (struct sockaddr*)&addr, &len);
        if(fd <= 0) 
            return;
        
        AddClient(fd, addr);
        printf("%s, %d\n", __func__, __LINE__);
    } while(listenEvent_ & EPOLLET);
}

void Server::AddClient(int fd, sockaddr_in addr)
{
    users_[fd].Init(fd, addr);
    if(timeoutMS_ > 0)
        timer_->Add(fd, timeoutMS_, std::bind(&Server::CloseConn, this, &users_[fd]));
    epoller_->AddFd(fd, EPOLLIN | connEvent_);
    SetNonblock(fd);
}

void Server::ExtentTime(HttpConn* client)
{
    if(timeoutMS_ > 0)
        timer_->Adjust(client->GetFd(), timeoutMS_);
}

void Server::CloseConn(HttpConn* client)
{
    printf("%s[%d], %d \n", __func__, client->GetFd(), __LINE__);
    epoller_->DelFd(client->GetFd());
    client->Close();
}