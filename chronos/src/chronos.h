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
#include "../../con2db/pgsql.h"

/* Local constants */

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define POSTGRESQL_SERVER "localhost"
#define POSTGRESQL_PORT "5432"
#define POSTGRESQL_USER "orchestrator"
#define POSTGRESQL_PSW "admin"
#define POSTGRESQL_DBNAME "chronos"

#define CONNECTION_REQUEST_STREAM "request-connection"
#define CONNECTION_ACCEPT_STREAM "ids-connection"

#define MIN_BLOCK 1 // milliseconds (0 == bocking call)
#define KEY_LEN 100
#define VALUE_LEN 100
#define QUERY_LEN 100

/* Types */



/* Classes */

class Chronos {
    private:
        int numProcesses;
        set<int> processIDs;

        long double simulationTime;

        set<int> activeProcesses;

        set<int> blockedProcesses;

        priority_queue<std::pair<long double, int>, std::vector<std::pair<long double, int>>, greater<std::pair<long double, int>>> syncProcessesTime;

        int upperRandInt;

        redisContext *c2r;

        redisReply *reply;

        PGresult *query_res;

        Con2DB db = Con2DB(POSTGRESQL_SERVER, POSTGRESQL_PORT, POSTGRESQL_USER, POSTGRESQL_PSW, POSTGRESQL_DBNAME);

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

        void handleTime();
};

#endif