#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sci.h"

#include "rcmanager.hpp"

extern "C" {

int filter_initialize(void **user_param)
{
    ResourceManager *rcManager = new ResourceManager();
    *user_param = rcManager;

    return SCI_SUCCESS;
}

int filter_terminate(void *user_param)
{
    ResourceManager *rcManager = (ResourceManager *)user_param;
    delete rcManager;

    return SCI_SUCCESS;
}

int filter_input(void *user_param, sci_group_t group, void *buf, int size)
{
    void *bufs[3] = {NULL, NULL, NULL};
    int sizes[3] = {0, 0, 0};
    int myID = -1;
    char rcMsg[1024] = {0};
    char ctl[16] = {0};
    int rc;

    ResourceManager *rcManager = (ResourceManager *)user_param;
    memcpy(ctl, (char *)buf + sizeof(int), sizeof(ctl));
    if (strncmp(ctl, "report", sizeof(ctl)) == 0) {
        int beID = ((int *) buf)[0];
        rc = SCI_Query(SCI_AGENT_ID, &myID);
        bufs[0] = &myID;
        sizes[0] = sizeof(myID);
        rcManager->setResource(beID, (char *)buf + sizeof(int) + sizeof(ctl), size);
        rcManager->getTotalMsg(rcMsg, sizeof(rcMsg));
        bufs[1] = ctl;
        sizes[1] = sizeof(ctl);
        bufs[2] = rcMsg;
        sizes[2] = strlen(rcMsg) + 1;

        rc = SCI_Filter_upload(SCI_FILTER_NULL, group, 3, bufs, sizes);
    } else if (strncmp(ctl, "inter", strlen("inter")) == 0) {
        int bestID = rcManager->getBestBranch();
        bufs[0] = buf;
        sizes[0] = size;
        rc = SCI_Filter_bcast(SCI_FILTER_NULL, 1, &bestID, 1, bufs, sizes);
    }

    return SCI_SUCCESS;
}

}
