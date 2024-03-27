#ifndef CHRONOS_H
#define CHRONOS_H

/* System libraries */

#include <string.h>
#include <vector>
#include <ctime>
#include <queue>
#include <set>
#include <random>
#include <cmath>
#include <iostream>

using namespace std;

/* Local libraries */

#include "../../con2redis/src/con2redis.h"

/* Local constants */

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define CONNECTION_REQUEST_STREAM "request-connection"
#define CONNECTION_ACCEPT_STREAM "ids-connection"

#define MIN_BLOCK 100
#define KEY_LEN 100
#define VALUE_LEN 100

/* Classes */

class Chronos {
    private:
        int numProcesses;
        set<int> processIDs;

        set<int> activeProcesses;

        set<int> blockedProcesses;

        priority_queue < pair<time_t, int>, vector<pair<time_t, int>>, greater<pair<time_t, int>> > syncProcessesTime;

        int upperRandInt;

        redisContext *c2r;

        redisReply *reply;

        int addProcess();

        int blockProcess(int pid);

        void unblockProcess(int pid);

        void handleDisconnection(int pid);

        void handleSynSleepReq(int pid);

    public:
        Chronos(int n);

        int getNumProcesses();

        int getNumBlockedProcesses();

        int getSizeActiveProcesses();

        int acceptIncomingConn();

        int handleEvents();

        void handleTime;
};

#endif