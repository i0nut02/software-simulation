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
#include <sstream>
#include <iomanip>
#include <limits>
#include <unordered_map>

using namespace std;

/* Local libraries */

#include "../../con2redis/src/con2redis.h"

/* Local constants */

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define CONNECTION_REQUEST_STREAM "request-connection"
#define CONNECTION_ACCEPT_STREAM "ids-connection"
#define RECEIVE_STREAM "orchestrator-in"
#define LOGGER_STREAM "chronos-logger"

#define MIN_BLOCK 1 // milliseconds (0 == bocking call)
#define KEY_LEN 100
#define VALUE_LEN 100
#define QUERY_LEN 100

#define NULL_VALUE -1

#define READ_CONNECTIONS "Read incoming connection requests"
#define SEND_ID "Send ID"
#define READ_STREAM "Read stream"
#define SYNC_PROCESS "Sync process"
#define FINISH_TIME 60 * 60 * 24 * 30 

/* Types */



/* Classes */

class Chronos {
    private:
        int numProcesses;
        int disconnectedProcesses;

        set<int> processIDs;

        long double simulationTime;

        set<int> activeProcesses;

        set<int> blockedProcesses;

        priority_queue<std::pair<long double, int>, std::vector<std::pair<long double, int>>, greater<std::pair<long double, int>>> syncProcessesTime;

        std::unordered_map<std::string, long double> ids;

        std::set<int> waitUnlockProcesses;

        int upperRandInt;

        redisContext *c2r;

        redisReply *reply;

        int waitUnlock = 0;

        int logLvl;

        int addProcess();

        int blockProcess(int pid);

        void unblockProcess(int pid);

        void handleDisconnection(int pid);

        void handleSynSleepReq(int pid);

        void logRedis(const char *stream, const char *message ,long double value);

    public:
        Chronos(int n, int logLvl = 3);

        int getNumProcesses();

        int getNumDisconnections();

        int getNumBlockedProcesses();

        int getSizeActiveProcesses();

        int acceptIncomingConn();

        int handleEvents();

        void handleTime();

        long int requests = 0;

        std::string getCurrentTime();
};
#endif