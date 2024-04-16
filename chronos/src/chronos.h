#ifndef CHRONOS_H
#define CHRONOS_H

/* System libraries */

#include <string.h>
#include <vector>
#include <ctime>
#include <queue>
#include <set>
#include <random>
#include <iostream>
#include <chrono>

using namespace std;

/* Local libraries */

#include "../../con2redis/src/con2redis.h"
#include "../../logger/src/logger.h"

/* Local constants */

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define CONNECTION_REQUEST_STREAM "request-connection"
#define CONNECTION_ACCEPT_STREAM "ids-connection"

#define MIN_BLOCK 1 // milliseconds (0 == bocking call)
#define KEY_LEN 100
#define VALUE_LEN 100
#define QUERY_LEN 100

#define NULL_VALUE -1
#define LOG_FILE "../../logfile.txt"

/* Types */



/* Classes */

class Chronos {
    private:
        int numProcesses;
        int disconnectedProcesses;
        Logger logger{LOG_FILE};

        set<int> processIDs;

        long double simulationTime;

        set<int> activeProcesses;

        set<int> blockedProcesses;

        priority_queue<std::pair<long double, int>, std::vector<std::pair<long double, int>>, greater<std::pair<long double, int>>> syncProcessesTime;

        int upperRandInt;

        redisContext *c2r;

        redisReply *reply;

        int addProcess();

        int blockProcess(int pid);

        void unblockProcess(int pid);

        void handleDisconnection(int pid);

        void handleSynSleepReq(int pid);

        void logRedis(const char *stream, const char *message ,long double value);

    public:
        Chronos(int n);

        int getNumProcesses();

        int getNumDisconnections();

        int getNumBlockedProcesses();

        int getSizeActiveProcesses();

        int acceptIncomingConn();

        int handleEvents();

        void handleTime();
};

#endif