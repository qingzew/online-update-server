#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
//extern char **environ;


class CRestart {
    public:
        CRestart();
        CRestart(char *argPath, char *argCmd, char *argArgs);
        ~CRestart();

        void SetPath(char *argPath);
        char *GetPath();
        void SetCmd(char *argCmd);
        char *GetCmd();
        void SetArgs(char *argArgs);
        char *GetArgs();
        void SetSignal();

    private:
        void Handler(int sig);

//        boost::function<void (int)> handlerFunc;
        char *path;
        char *cmd;
        char *args;
};


