#ifndef _NETLAYER_HPP
#define _NETLAYER_HPP

#include <string>
#include <sci.h>

#include "command.hpp"

#define SCHEDULE_FILTER 1
#define SCHEDULE_SO_FILE "/opt/cloudland/lib64/scheduler.so"

using namespace std;

class NetLayer {
    private:
        sci_info_t sciInfo;
        string bePath;
        string hFile;

    public:
        NetLayer();
        int initFE(char *backend, char *hostfile);
        int startBE();
        int sendMessage(struct Command &cmd, bool toAll = false);
        int sendMessage(int beID, struct Command &cmd);
        ~NetLayer();
        
};

#endif
