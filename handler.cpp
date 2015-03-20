#include <string.h>
#include <stdio.h>

#include <string>

#include "handler.hpp"
#include "netlayer.hpp"
#include "command.hpp"
#include "log.hpp"

using namespace std;

const char * RESCUE_CMD = "/opt/cloudland/scripts/frontend/rescue.sh";

int exec_cmd(int id, char *cmd)
{
    int bytes = 0;
    FILE *fp = NULL;
    char result[1024] = {0};

    fp = popen(cmd, "r");
    bytes =  fread(result, sizeof(char), sizeof(result) - 1, fp);
    pclose(fp);
    log_info("Backend or agent %d responded command %s, result %s", id, cmd, result);

    return 0;
}

void frontHandler(void *user_param, sci_group_t group, void *buffer, int size)
{
    int be_id = ((int *) buffer)[0];
    char ctl[16] = {0};
    char *msg = (char *)((char *) buffer + sizeof(int) + sizeof(ctl));
    int tail = size - sizeof(be_id) - sizeof(ctl) - 2;

    memcpy(ctl, (char *) buffer + sizeof(int), sizeof(ctl));
    if (msg[tail] == '\n') {
        msg[tail] = '\0';
    }

    if (strncmp(ctl, "backres", sizeof(ctl)) == 0) {
        char *cmd = NULL; 

        log_info("Cloudlet %d responded message %s", be_id, msg);
        cmd = strstr(msg, "|:-COMMAND-:|");
        if (cmd == NULL) 
            return;

        cmd += strlen("|:-COMMAND:-|") + 1;
        exec_cmd(be_id, cmd);
    } else if (strncmp(ctl, "rescue", sizeof(ctl)) == 0) {
        char cmd[256] = {0};
        snprintf(cmd, sizeof(cmd), "%s %s", RESCUE_CMD, msg);
        exec_cmd(be_id, cmd);
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
        char ctl[16] = "backres";
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
            rc = SCI_Upload(SCI_FILTER_NULL, group, 3, bufs, sizes);
        }
    }
}
