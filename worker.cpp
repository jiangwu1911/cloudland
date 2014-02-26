#include "worker.hpp"
#include "command.hpp"
#include "log.hpp"

#define CLOUDLET_PATH "/opt/cloudland/bin/cloudlet"
#define CLOUD_HOST_FILE "/opt/cloudland/etc/host.list"

CloudWorker::CloudWorker()
{
}

CloudWorker::~CloudWorker()
{
}

int CloudWorker::init()
{
    unixSock.init();
    sciNet.initFE(CLOUDLET_PATH, CLOUD_HOST_FILE);

    return 0;
}

int CloudWorker::execute(Command &cmd, char *result, int size)
{
    int n = 0;
    FILE *fp = NULL;

    memset(result, '\0', size);
    fp = popen(cmd.content, "r");
    n = fread(result, sizeof(char), size - 1, fp);
    fclose(fp);

    return 0;
}

int CloudWorker::parse(Command &command)
{
    if (strncasecmp(command.control, "exec", strlen("exec")) == 0) {
        char result[1024] = {0};

        execute(command, result, sizeof(result));
        log_info("Command %s was executed with result %s", command.content, result);
    } else if (strncasecmp(command.control, "inter", strlen("inter")) == 0) {
        int node = -1;
        char *pnode = strstr(command.control, " ");

        if (pnode != NULL) {
            while (*pnode == ' ')
                pnode++;

            if (*pnode != '\0')
                node = atoi((const char *)pnode);
        }
        if (node >= 0) {
            sciNet.sendMessage(node, command);
            log_info("Command %s was sent to node %d", command.content, node);
        } else {
            sciNet.sendMessage(command);
            log_info("Command %s was sent to scheduler", command.content);
        }
    } else if (strncasecmp(command.control, "toall", strlen("toall")) == 0) {
            sciNet.sendMessage(command);
            log_info("Command %s was sent to all backend", command.content);
    }
    delete [] command.content;

    return 0;
}

int CloudWorker::work()
{
    struct Command command = {0};

    while (true) {
        unixSock.poll(command);
        parse(command);
    }

    return 0;
}
