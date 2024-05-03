#ifndef CHRONOSLIB
#define CHRONOSLIB

#include <string>
#include <iostream>
#include <cstring>

#include "../../con2redis/src/con2redis.h"
#include "../../logger/src/logger.h"

#define REQUEST_CONNECTION "request-connection"
#define IDS_CONNECTION "ids-connection"
#define SEND_STREAM "orchestrator-in"

#define LOG_FILE "../../../logfile.txt"

#define CONN_REQ 5
#define WAIT_ID 6
#define BLOCKING_CALL 7
#define UNBLOCK 8
#define SYN_SLEEP 9
#define MY_SLEEP 10
#define WAIT_SYNC 11
#define DISCONNECT 12

#define VALUE_LEN 5000
#define KEY_LEN 100
#define ORCHERTRATOR_ID 0
#define NULL_PARAM -1

extern int _pid;
extern int _logLvl;
extern long double _currentTimestamp;

extern redisContext *_c2r;
extern redisReply *_reply;
extern Logger _logger;

int connect(char *redisIP = const_cast<char*>("localhost"), int redisPort = 6379, int logLvl = 3);

void alertBlocking();

void unblock();

void synSleep(long double T);

void mySleep(long double T);

void disconnect();

void logRedis(const char *stream, int message ,long double value);

long double getSimulationTimestamp();

#endif