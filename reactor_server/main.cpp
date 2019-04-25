#include <ReactorServer.h>

#include <iostream>
#include <signal.h>     //for signal()
#include <unistd.h>
#include <stdlib.h>       //for exit()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Log.h"

using std::cout;
using std::endl;

ReactorServer g_server;

void prog_exit(int signo) {
    cout << "Program reav signal: " << signo << " to exit." << endl;
    g_server.uninit();
}

void daemon_run() {

    signal(SIGCHLD, SIG_IGN);
    int pid = fork();
    if (pid < 0) {
        cout << "Fork error." << endl;
        exit(-1);
    } else if (pid > 0) {
        exit(0);
    }

    setsid();
    int fd = open("/dev/null", O_RDWR, 0);
    if (-1 != fd) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }
    if (fd > 2) {
        close(fd);
    }
}

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;

    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGKILL, prog_exit);
    signal(SIGTERM, prog_exit);

    short port = 0;
    int ch;
    bool bdaemon = false;

    while (-1 != (ch = getopt(argc, argv, "p:d"))) {
        switch (ch) {
            case 'd':
                bdaemon = true;
                break;
            case 'p':
                port = atol(optarg);
                break;
        }
    }

    ServerLog::GetInstance().Start();

    if (bdaemon) daemon_run();
    if (0 == port) port = 12345;
    if (!g_server.init("0.0.0.0", port)) return -1;
    ReactorServer::main_loop(&g_server);

    return 0;
}