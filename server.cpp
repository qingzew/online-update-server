#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <fstream>
#include <ctime>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#define MAXLINE 4096

char *path = NULL;
char *cmd = NULL;

int listenfd = -1, connfd = -1;

void Handler(int sig) {
    openlog("myServer", LOG_CONS | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "processing signal...");

    char args[1024];
    sprintf(args, "%d", listenfd);

    pid_t fpid = fork();
    if (fpid < 0 ) {
        syslog(LOG_ERR, "fork error: %s(errno: %d)\n", strerror(errno), errno);
        exit(EXIT_FAILURE);
    } else if (fpid == 0) {
        if (args == "-1") {
            execle(path, cmd, NULL, environ);
        } else {
            execle(path, cmd, args, NULL, environ);
        }

    } else {
        syslog(LOG_INFO, "parent exit...");
    }

    closelog();
}

void Child(int sockFd) {
    char buff[4096];
    int n;

    n = recv(sockFd, buff, MAXLINE, 0);
    buff[n] = '\0';

    std::ofstream out("/tmp/output", std::ios::app);
    std::time_t result = std::time(0);
    out<<boost::this_thread::get_id()<<" "<<buff<<" "<<std::asctime(std::localtime(&result));
    out.close();
}


void SetSig() {
    openlog("myServer", LOG_CONS | LOG_PID, LOG_USER);

    struct sigaction action;
    action.sa_handler = Handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    //    action.sa_flags |= SA_RESTART;

    if (sigaction(SIGALRM, &action, NULL) < 0) {
        syslog(LOG_ERR, "set signal error: %s(errno: %d)\n", strerror(errno), errno);
    } else {
        syslog(LOG_ERR, "set signal ok");
    }
    closelog();
    //    alarm(3);
}

void NewConnect() {
    openlog("myServer", LOG_CONS | LOG_PID, LOG_USER);

    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        syslog(LOG_ERR, "create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }

    //    timeval timeout = {1, 0};
    //    if (setsockopt(listenfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeval)) < 0)
    //    {
    //        exit(1);
    //    }
    //
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        syslog(LOG_ERR, "bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(EXIT_SUCCESS);
    }

    if(listen(listenfd, 10) == -1){
        syslog(LOG_ERR, "listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(EXIT_FAILURE);
    }


    while(1){
        if((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            if (errno == EINTR) {
                syslog(LOG_ERR, "parent, accept error: %s(errno: %d)\n", strerror(errno), errno);
                break;
            }
        }

        boost::thread thrd(boost::bind(&Child, listenfd));
        thrd.join();
        close(connfd);
    }

    closelog();
}

void NewDaemon() {
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
//    if ((chdir("/")) < 0) {
//        exit(EXIT_FAILURE);
//    }

    //close standard file description
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);


}


void NewListen() {
    openlog("myServer", LOG_CONS | LOG_PID, LOG_USER);

    while(1){
        if((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            if (errno == EINTR) {
                syslog(LOG_ERR, "accept error: %s(errno: %d)\n", strerror(errno), errno);
                break;
            }
        }

        boost::thread thrd(boost::bind(&Child, listenfd));
        thrd.join();
        close(connfd);
    }

    closelog();
}

int main(int argc, char const* argv[])
{
    openlog("myServer", LOG_CONS | LOG_PID, LOG_USER);

    syslog(LOG_ERR, "arg num:  %d\n", argc);
    //main process

    path = const_cast<char *>(argv[0]);
    cmd = const_cast<char *>(argv[0]);


    if (argc == 1) {
        NewDaemon();
        SetSig();
        NewConnect();
    } else if (argc == 2) {
        listenfd = atoi(argv[1]);
        SetSig();
//        NewListen();
        while(1){}
    } else {
        syslog(LOG_ERR, "args error..");
        exit(EXIT_FAILURE);
    }


    //   close(listenfd);

    //    close the log
    closelog();

    return 0;
}
