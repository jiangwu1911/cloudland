#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "rcmanager.hpp"

ResourceManager::ResourceManager()
    : updated(false)
{
    memset(&totalRC, 0, sizeof(totalRC));
}

ResourceManager::~ResourceManager()
{
    map<int, struct Resource *>::iterator it = rcMap.begin();
    for (; it != rcMap.end(); ++it) {
        delete it->second;
    }
    rcMap.clear();
}

int ResourceManager::getBestBranch()
{
    int bestID = -1;
    int tmp = 0;
    int weight = 0;

    while (!updated) {
        usleep(200);
    }
    map<int, struct Resource *>::iterator it = rcMap.begin();
    for (; it != rcMap.end(); ++it) {
        tmp = it->second->memory + it->second->disk + it->second->cpu * 100 + (2000000 - it->second->bw_RX - it->second->bw_TX) + (200000 - it->second->io_RD - it->second->io_WT);
        if (tmp > weight) {
            weight = tmp;
            bestID = it->first;
        }
    }
    updated = false;

    return bestID;
}

int ResourceManager::setResource(int id, char *rcMsg, int size)
{
    char *p = rcMsg;

    Resource *resource = new Resource();
    memset(resource, 0, sizeof(Resource));
    if (p != NULL)
        resource->memory = atoi(p);
    p = strstr(p, " ");
    if (p != NULL)
        resource->disk = atoi(p);
    p = strstr(p, " ");
    if (p != NULL)
        resource->cpu = atoi(p);
    p = strstr(p, " ");
    if (p != NULL)
        resource->bw_RX = atoi(p);
    p = strstr(p, " ");
    if (p != NULL)
        resource->bw_TX = atoi(p);
    p = strstr(p, " ");
    if (p != NULL)
        resource->io_RD = atoi(p);
    p = strstr(p, " ");
    if (p != NULL)
        resource->io_WT = atoi(p);

    rcMap[id] = resource;
    totalResource();
    updated = true;

    return 0;
}

int ResourceManager::totalResource()
{
    memset(&totalRC, 0, sizeof(totalRC));

    map<int, struct Resource *>::iterator it = rcMap.begin();
    for (; it != rcMap.end(); ++it) {
        totalRC.memory += it->second->memory;
        totalRC.disk += it->second->memory;
        totalRC.cpu += it->second->cpu;
        totalRC.bw_RX += it->second->bw_RX;
        totalRC.bw_TX += it->second->bw_TX;
        totalRC.io_RD += it->second->io_RD;
        totalRC.io_WT += it->second->io_WT;
    }

    return 0;
}

int ResourceManager::getTotalMsg(char *rcMsg, int size)
{
    memset(rcMsg, '\0', size);
    snprintf(rcMsg, size-1, "%d %d %d %d %d %d %d\n", totalRC.memory, totalRC.disk, totalRC.cpu, totalRC.bw_RX, totalRC.bw_TX, totalRC.io_RD, totalRC.io_WT);

    return 0;
}
