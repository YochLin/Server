#include <sys/socket.h>
#include <netinet/in.h>      // sockaddr_in
#include <arpa/inet.h>
#include <netdb.h>          // gethostbyname()
#include <unistd.h>         // close()
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char *url = "localhost";

int main()
{
    uint64_t inaddr;
    int32_t sockfd;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8484);

    struct hostent *hp;
    inaddr = inet_addr(url);
    if(inaddr != INADDR_NONE){
        memcpy(&addr.sin_addr, &inaddr, sizeof(inaddr));
    }
    else{
        hp = gethostbyname(url);
        if(hp == NULL){
            printf("Url is invalid\n");
            return -1;
        }

        memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        printf("Error socket\n");
        close(sockfd);
        return -1;
    }

    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        printf("connect wrong\n");
        close(sockfd);
        return -1;
    }

    printf("Connect successful!\n");
    shutdown(sockfd, SHUT_RDWR);
    printf("Disconnect server!\n");
    close(sockfd);

    return 0;
}