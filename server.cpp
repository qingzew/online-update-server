#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fstream>
#include <ctime>
#include <iostream>


#define MAXLINE 4096

int main(int argc, char const* argv[])
{

    pid_t pid, sid;

    //fork the parent process
    pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    //we got a good pid, close the parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    //change file mask
    umask(0);

    //create a new signature id for our child
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    //change directory
    //if we can not find the directory we exit with failure
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    //close standard file description
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    //main process
    int    listenfd, connfd;
    struct sockaddr_in     servaddr;
    char    buff[4096];
    int     n;

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    if( listen(listenfd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    printf("======waiting for client's request======\n");
    while(1){
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
//        printf("recv msg from client: %s\n", buff);

        std::time_t result = std::time(0);

        std::ofstream out("/tmp/output", std::ios::app);
        out<<buff<<" "<<std::asctime(std::localtime(&result));
        out.close();

        close(connfd);
    }

    close(listenfd);

	//close the log
	closelog();

	return 0;
}
