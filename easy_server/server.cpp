#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t port = 8484;

int main()
{
    int32_t sockfd;     // 描述符
    int32_t ret;
    int32_t connectfd;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        printf("Socket wrong!\n");
        close(sockfd);
        return 0;
    }

    ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0){
        printf("Bind Port: %d error!\n", port);
        close(sockfd);
        return 0;
    }

    ret = listen(sockfd, 6);
    if(ret < 0){
        close(sockfd);
        return 0;
    }

    while(1){
        connectfd = accept(sockfd, NULL, NULL);

        if(0 < connectfd){
            close(sockfd);
            break;
        }

        shutdown(connectfd, SHUT_RDWR);
        close(connectfd);
    }
    close(sockfd);
    return 0;
}