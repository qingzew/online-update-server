#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>

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

        void Handler(int sig);
        void SetSignal();
    private:
        char *path;
        char *cmd;
        char *args;
};


