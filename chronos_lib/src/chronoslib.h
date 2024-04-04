#ifndef CHRONS_LIB
#define CHRONOS_LIB

#include <string>
#include <iostream>
#include <cstring>

#include "../../con2redis/src/con2redis.h"
#include "../../con2db/pgsql.h"

#define POSTGRESQL_SERVER "localhost"
#define POSTGRESQL_PORT "5432"
#define POSTGRESQL_USER "orchestrator"
#define POSTGRESQL_PSW "admin"
#define POSTGRESQL_DBNAME "redis_log"

#define VALUE_LEN 5000
#define ORCHERTRATOR_ID 0
#define NULL_PARAM -1

extern int _pid;
extern redisContext *_c2r;
extern redisReply *_reply;
extern Con2DB _db;
extern PGresult *_query_res;

int connect(char *redisIP = const_cast<char*>("localhost"), int redisPort = 6379);

void synSleep(long double T);

void alertBlocking();

void mySleep(long double T);

void disconnect();

int logRedis(int from, int to, const char *action, long double reqVal);

#endif