#include <unistd.h>         // close() function
#include <sys/socket.h>
#include <netinet/in.h>

#include "eopller.h"

class Server
{
public:
    explicit Server(int port);
    ~Server();

    void Start();

private:
    bool InitStocket();
    void AddClient();

    void DealListen();

private:

    int port_;
    int listenFd_;
    bool openLinger_;
    int timeoutMS_;
    bool isClose_;
    
    uint32_t listenEvent_;
    uint32_t connEvent_;
};