#include <string.h>
#include <stdio.h>

#include <string>

#include "handler.hpp"
#include "netlayer.hpp"
#include "command.hpp"
#include "log.hpp"

using namespace std;

void frontHandler(void *user_param, sci_group_t group, void *buffer, int size)
{
    int bytes = 0;
    int be_id = ((int *) buffer)[0];
    char ctl[16] = {0};
    char *msg = (char *)((char *) buffer + sizeof(int) + sizeof(ctl));
    int tail = size - sizeof(be_id) - sizeof(ctl) - 2;
    char result[1024] = {0};
    FILE *fp = NULL;

    memcpy(ctl, (char *) buffer + sizeof(int), sizeof(ctl));
    if (msg[tail] == '\n') {
        msg[tail] = '\0';
    }

    if (strncmp(ctl, "backcmd", sizeof(ctl)) == 0) {
        fp = popen(msg, "r");
        bytes =  fread(result, sizeof(char), sizeof(result) - 1, fp);
        pclose(fp);
        log_info("Cloudlet %d responded command %s", be_id, msg);
    } else if (strncmp(ctl, "backres", sizeof(ctl)) == 0) {
        log_info("Cloudlet %d responded message %s", be_id, msg);
    }
}

void backHandler(void *user_param, sci_group_t group, void *buffer, int size)
{
    char *p = (char *)buffer;
    int ctLen = 0;
    struct Command cmd = {0};

    strncpy(cmd.format, p, sizeof(cmd.format));
    p += sizeof(cmd.format);
    strncpy(cmd.control, p, sizeof(cmd.control));
    p += sizeof(cmd.control);
    ctLen = size - (sizeof(cmd.format) + sizeof(cmd.control));
    if (ctLen > 0) {
        cmd.content = p;
    }

    if ((strncmp(cmd.format, "raw", sizeof(cmd.format)) == 0) && 
            (strncmp(cmd.control, "inter", strlen("inter")) == 0)) {
        int bytes, my_id, rc;
        char ctl[16] = "backcmd";
        void *bufs[3] = {NULL, ctl, NULL};
        int sizes[3] = {0, sizeof(ctl), 0};
        char result[1024] = {0};
        FILE *fp = NULL;
        string cmdStr = cmd.content;

        rc = SCI_Query(BACKEND_ID, &my_id);
        if (rc != SCI_SUCCESS) {
            return;
        }

        bufs[0] = &my_id;
        sizes[0] = sizeof(my_id);
        cmdStr = "sudo " + cmdStr + " 2>&1";
        fp = popen(cmdStr.c_str(), "r");
        bytes = fread(result, sizeof(char), sizeof(result) - 1, fp);
        fclose(fp);
        if (bytes > 0) {
            sizes[2] = bytes + 1;
            bufs[2] = result;
            if (strstr(result, "frontback") == NULL) {
                memset(ctl, '\0', sizeof(ctl));
                strncpy(ctl, "backres", sizeof(ctl) - 1);
            }
            rc = SCI_Upload(SCHEDULE_FILTER, group, 3, bufs, sizes);
        }
    }
}
