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
    int weight = 0;

    while (!updated) {
        usleep(200);
    }
    map<int, struct Resource *>::iterator it = rcMap.begin();
    for (; it != rcMap.end(); ++it) {
        if (it->second->avaibility > weight) {
            weight = it->second->avaibility;
            bestID = it->first;
        }
    }
    updated = false;

    return bestID;
}

int ResourceManager::setAvailibility(int id, int avail)
{
    Resource *resource = new Resource();

    memset(resource, 0, sizeof(Resource));
    resource->avaibility = avail;
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
        totalRC.avaibility += it->second->avaibility;
    }

    return 0;
}

int ResourceManager::getTotalMsg(char *rcMsg, int size)
{
    memset(rcMsg, '\0', size);
    snprintf(rcMsg, size-1, "%d\n", totalRC.avaibility);

    return 0;
}
