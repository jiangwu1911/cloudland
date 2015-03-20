#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <sci.h>
#include <errno.h>
#include <unistd.h>

#include <fstream>

#include "handler.hpp"
#include "netlayer.hpp"
#include "log.hpp"

const char * REPORT_RC_CMD = "/opt/cloudland/scripts/backend/report_rc.sh";

string pidFile;
string hostList;

void set_oom_adj(int s)
{
    char oomfname[256] = "";
    char oomadjstr[16] = "";
    int oomfd = -1;
    struct stat oomadjst = {0};
    int nbytes = -1;

    int score = s;
    score = (score < -1000) ? (-1000) : (score);
    score = (score > 1000) ? (1000) : (score);

    ::sprintf(oomfname, "/proc/%d/oom_score_adj", getpid());
    if (::stat(oomfname, &oomadjst) != 0) {
        /* could not find oom_score_adj, will try oom_adj instead */
        ::sprintf(oomfname, "/proc/%d/oom_adj", getpid());
        double oomadjval = (score < 0) ? (((double) score/1000.0)*17.0) : (((double) score/1000.0)*15.0);
        ::sprintf(oomadjstr, "%.0f", oomadjval);
    } else {
        ::sprintf(oomadjstr, "%d", score);
    }

    oomfd = ::open(oomfname, O_WRONLY, 0);
    if (oomfd < 0) {
        log_error("open() failed for %s: errno = %d\n", oomfname, errno);
        return;
    }

    nbytes = ::write(oomfd, oomadjstr, strlen(oomadjstr));
    if (nbytes < 0) {
        log_error("write() failed for %s: errno = %d", oomfname, errno);
    } else {
        log_crit("wrote %d bytes to %s: %s", nbytes, oomfname, oomadjstr);
    }
    ::close(oomfd);
}

int main(int argc, char *argv[])
{
    int rc, bytes, myID, status;
    char result[1024] = {0};
    char ctl[16] = "report";
    FILE *fp = NULL;
    void *bufs[3] = {NULL, ctl, NULL};
    int sizes[3] = {0, sizeof(ctl), 0};
    sigset_t sigs_to_block;
    sigset_t old_sigs;

    set_oom_adj(-1000);

    NetLayer netLayer;
    netLayer.startBE();

    rc = SCI_Query(BACKEND_ID, &myID);
    if (rc != SCI_SUCCESS) {
        exit(-1);
    }
    sigemptyset(&sigs_to_block);
    pthread_sigmask(SIG_SETMASK, &sigs_to_block, &old_sigs);

    bufs[0] = &myID;
    sizes[0] = sizeof(myID);
    setsid();
    fp = popen(REPORT_RC_CMD, "r");
    rc = SCI_Query(HEALTH_STATUS, &status);
    while (status == 0) {
        char *p = NULL;
        memset(result, '\0', sizeof(result));
        p = fgets(result, sizeof(result) - 1, fp);
        if (p == NULL) {
            pclose(fp);
            fp = popen(REPORT_RC_CMD, "r");
            p = fgets(result, sizeof(result) - 1, fp);
        }
        sizes[2] = strlen(result);
        bufs[2] = result;
        rc = SCI_Upload(SCHEDULE_FILTER, SCI_GROUP_ALL, 3, bufs, sizes);
        rc = SCI_Query(HEALTH_STATUS, &status);
    }
    kill(-getpid(), 9);
    pclose(fp);

    return 0;
}
