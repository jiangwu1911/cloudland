#ifndef _UNIXSOCK_HPP
#define _UNIXSOCK_HPP

#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

#include <string>

#include "command.hpp"

using namespace std;

class UnixSocket {
    private:
        int socket;
        string sockFile;
        struct sockaddr_un srvaddr;

    public:
        UnixSocket();
        ~UnixSocket();
        int init();
        int poll(struct Command &cmd);
        int send(struct Command &cmd);
};

#endif
