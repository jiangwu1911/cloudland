#ifndef _RCMANAGER_HPP
#define _RCMANAGER_HPP

#include <map>
#include <string>

using namespace std;

struct Resource {
    int avaibility;
    int hitRate;
};

class ResourceManager {
    private:
        map<int, struct Resource *> rcMap;
        Resource totalRC;
        bool updated;

    public:
        ResourceManager();
        ~ResourceManager();
        int setAvailibility(int id, int avail);
        int totalResource();
        int getBestBranch();
        int getBest();
        int getTotalMsg(char *rcMsg, int size);
};

#endif
