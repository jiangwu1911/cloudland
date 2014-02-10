#ifndef _WORKER_HPP
#define _WORKER_HPP

#include "netlayer.hpp"
#include "unixsock.hpp"
#include "command.hpp"

class CloudWorker {
    private:
        NetLayer sciNet;
        UnixSocket unixSock;

    public:
        CloudWorker();
        ~CloudWorker();
        int parse(Command &cmd);
        int execute(Command &cmd, char *result, int size);
        int init();
        int work();
};

#endif
