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

int upload_message(int group, char *cmd, char *msg)
{
    int rc = 0;
    void *bufs[3] = {NULL, NULL, NULL};
    int sizes[3] = {0, 0, 0};
    char ctl[16] = {0};
    int myID = -1;

    rc = SCI_Query(SCI_AGENT_ID, &myID);
    bufs[0] = &myID;
    sizes[0] = sizeof(myID);
    strncpy(ctl, cmd, sizeof(ctl));
    bufs[1] = ctl;
    sizes[1] = sizeof(ctl);
    bufs[2] = msg;
    sizes[2] = strlen(msg) + 1;
    rc = SCI_Filter_upload(SCI_FILTER_NULL, group, 3, bufs, sizes);

    return rc;
}

int report_availibility(int group, ResourceManager *rcMgr)
{
    int rc = 0;
    char rcMsg[1024] = {0};
    char ctl[16] = "report";

    rcMgr->getTotalMsg(rcMsg, sizeof(rcMsg));
    rc = upload_message(group, ctl, rcMsg);

    return rc;
}

int filter_input(void *user_param, sci_group_t group, void *buf, int size, sci_exflag_t *exflag)
{
    char ctl[16] = {0};
    int rc;

    ResourceManager *rcManager = (ResourceManager *)user_param;
    if (exflag->flag == SCI_ERROR_CHILD) {
        int num = 0;
        int *errIDs = NULL;

        rc = SCI_Query(NUM_ERROR_SUCCESSORS, &num);
        errIDs = new int[num];
        rc = SCI_Query(ERROR_SUCCESSORS, errIDs);
        for (int i = 0; i < num; i++) {
            char eID[32] = {0};
            rcManager->setAvailibility(errIDs[i], 0);
            snprintf(eID, sizeof(eID), "%d\n", errIDs[i]);
            upload_message(group, "rescue", eID);
        }
        report_availibility(group, rcManager);
    } else if (buf != NULL) {
        memcpy(ctl, (char *)buf + sizeof(int), sizeof(ctl));
        if (strncmp(ctl, "report", sizeof(ctl)) == 0) {
            char *p = NULL;
            int avail = 0;
            int beID = ((int *) buf)[0];
            p = (char *)buf + sizeof(int) + sizeof(ctl);
            if (p != NULL) {
                avail = atoi(p);
            }
            rcManager->setAvailibility(beID, avail);
            report_availibility(group, rcManager);
        } else if (strncmp(ctl, "inter", strlen("inter")) == 0) {
            int bestID = rcManager->getBestBranch();
            rc = SCI_Filter_bcast(SCI_FILTER_NULL, 1, &bestID, 1, &buf, &size);
        }
    }

    return SCI_SUCCESS;
}

}
