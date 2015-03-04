#include "restart.h"

CRestart::CRestart() {
    path = NULL;
    cmd = NULL;
    args = NULL;
}

CRestart::CRestart(char *argPath, char *argCmd, char *argArgs) {
    path = argPath;
    cmd = argCmd;
    args = argArgs;
}

CRestart::~CRestart() {

}

void CRestart::SetPath(char *argPath) {
    path = argPath;
}

char *CRestart::GetPath() {
    return path;
}

void CRestart::SetCmd(char *argCmd) {
    cmd = argCmd;
}

char *CRestart::GetCmd(){
    return cmd;
}

void CRestart::SetArgs(char *argArgs) {
    args = argArgs;
}

char *CRestart::GetArgs() {
    return args;
}

void CRestart::Handler(int sig) {
    pid_t fpid = fork();
    if (fpid < 0 ) {
        std::cout<< "error in fork..."<<std::endl;
        exit(1);
    } else if (fpid == 0) {
        if (args == NULL) {
            execle(path, cmd, NULL, environ);
        } else {
            execle(path, cmd, args, NULL, environ);
        }

    } else {
        std::cout<<"parent exit...";
    }
}

//void CRestart::SetSignal() {
//    struct sigaction action;
//    action.sa_handler = &Handler;
//    sigemptyset(&action.sa_mask);
//    action.sa_flags = 0;
////    action.sa_flags |= SA_RESTART;
//
//    sigaction(SIGALRM, &action, NULL);
//}


int main (int argc, char **argv) {
    CRestart cr(argv[0], argv[0], argv[1]);

    std::cout<<cr.GetPath()<<std::endl;
    std::cout<<cr.GetCmd()<<std::endl;
    std::cout<<cr.GetArgs()<<std::endl;

    struct sigaction action;
    action.sa_handler = cr.Handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
//    action.sa_flags |= SA_RESTART;

    sigaction(SIGALRM, &action, NULL);

    return 0;
}
