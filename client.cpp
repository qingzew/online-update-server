#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sys/time.h> //struct itimerval, setitimer()
#include <signal.h> //signal()

#define MAXLINE 4096

void SendMsg(int sig)
{
    printf("send message...\n");

    int sockfd, n;

    struct sockaddr_in servaddr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n", "127.0.0.1");
        exit(0);
    }

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    if(send(sockfd, "test", strlen("test"), 0) < 0)
    {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

   close(sockfd);
}

void GetTimer() {
    struct itimerval tick;

    signal(SIGALRM, SendMsg);
    memset( &tick, 0, sizeof(tick) );

    tick.it_value.tv_sec = 1;
    tick.it_value.tv_usec = 0;
    tick.it_interval.tv_sec = 1;
    tick.it_interval.tv_usec = 0;

    int res = setitimer(ITIMER_REAL, &tick, NULL);
    if(res) {
        printf( "set timer failed\n" );
    }
}


int main(int argc, char** argv)
{
    GetTimer();

    while (1) {
        pause();
    }

    return 0;
}
