#include <string.h>
#include <string>

#include "netlayer.hpp"
#include "handler.hpp"
#include "exception.hpp"

using namespace std;

NetLayer::NetLayer()
{
}

NetLayer::~NetLayer()
{
    SCI_Terminate();
}

int NetLayer::initFE(char *backend, char *hostfile)
{
    int rc;
    sci_filter_info_t filter = {SCHEDULE_FILTER, SCHEDULE_SO_FILE};
    sci_filter_list_t flist = {1, &filter};

    bePath = backend;
    hFile = hostfile;
    memset(&sciInfo, 0, sizeof(sciInfo));
    sciInfo.type = SCI_FRONT_END;
    sciInfo.fe_info.mode = SCI_INTERRUPT;
    sciInfo.fe_info.hostfile = (char *)hFile.c_str();
    sciInfo.fe_info.bepath = (char *)bePath.c_str();
    sciInfo.fe_info.hndlr = (SCI_msg_hndlr *)&frontHandler;
    sciInfo.fe_info.filter_list = flist;
    sciInfo.fe_info.param = NULL;

    rc = SCI_Initialize(&sciInfo);
    if (rc != SCI_SUCCESS) {
        throw CommonException(CommonException::SCI_INIT_ERROR);
    }

    return 0;
}

int NetLayer::startBE()
{
    int rc;

    memset(&sciInfo, 0, sizeof(sciInfo));
    sciInfo.type = SCI_BACK_END;
    sciInfo.be_info.mode = SCI_INTERRUPT;
    sciInfo.be_info.hndlr = (SCI_msg_hndlr *)&backHandler;
    sciInfo.be_info.param = NULL;

    rc = SCI_Initialize(&sciInfo);
    if (rc != SCI_SUCCESS) {
        throw CommonException(CommonException::SCI_INIT_ERROR);
    }

    return 0;
}

int NetLayer::sendMessage(int beID, struct Command &cmd)
{
    int rc = -1;
    void *bufs[3];
    int sizes[3];
    int nVec = 2;

    bufs[0] = cmd.format;
    sizes[0] = sizeof(cmd.format);
    bufs[1] = cmd.control;
    sizes[1] = sizeof(cmd.control);
    if (cmd.size > 0) {
        nVec = 3;
        bufs[2] = cmd.content;
        sizes[2] = cmd.size;
    }
    rc = SCI_Bcast(SCI_FILTER_NULL, beID, nVec, bufs, sizes);
    if (rc != SCI_SUCCESS) {
        throw CommonException(CommonException::SCI_BCAST_ERROR); 
    }

    return 0;
}

int NetLayer::sendMessage(struct Command &cmd, bool toAll)
{
    int rc = -1;
    void *bufs[3];
    int sizes[3];
    int nVec = 2;

    bufs[0] = cmd.format;
    sizes[0] = sizeof(cmd.format);
    bufs[1] = cmd.control;
    sizes[1] = sizeof(cmd.control);
    if (cmd.size > 0) {
        nVec = 3;
        bufs[2] = cmd.content;
        sizes[2] = cmd.size;
    }
    if (toAll) {
        rc = SCI_Bcast(SCI_FILTER_NULL, SCI_GROUP_ALL, nVec, bufs, sizes);
    } else {
        rc = SCI_Bcast(SCHEDULE_FILTER, SCI_GROUP_ALL, nVec, bufs, sizes);
    }
    if (rc != SCI_SUCCESS) {
        throw CommonException(CommonException::SCI_BCAST_ERROR); 
    }

    return 0;
}

