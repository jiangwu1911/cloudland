#ifndef _RCMANAGER_HPP
#define _RCMANAGER_HPP

#include <map>
#include <string>

using namespace std;

struct Resource {
    int memory;
    int disk;
    int cpu;
    int bw_RX;
    int bw_TX;
};

class ResourceManager {
    private:
        map<int, struct Resource *> rcMap;
        Resource totalRC;

    public:
        ResourceManager();
        ~ResourceManager();
        int setResource(int id, char *rcMsg, int size);
        int totalResource();
        int getBestBranch();
        int getBest();
        int getTotalMsg(char *rcMsg, int size);
};

#endif
