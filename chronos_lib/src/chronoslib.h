#ifndef CHRONOSLIB
#define CHRONOSLIB

#include <string>
#include <iostream>
#include <cstring>

#include "../../con2redis/src/con2redis.h"
#include "../../logger/src/logger.h"

#define REQUEST_CONNECTION "request-connection"
#define IDS_CONNECTION "ids-connection"

#define LOG_FILE "../../../logfile.txt"

#define VALUE_LEN 5000
#define ORCHERTRATOR_ID 0
#define NULL_PARAM -1

extern int _pid;
extern redisContext *_c2r;
extern Logger _logger;

int connect(char *redisIP = const_cast<char*>("localhost"), int redisPort = 6379);

void synSleep(long double T);

void alertBlocking();

void mySleep(long double T);

void disconnect();

void logRedis(const char *stream, const char *message ,long double value);

#endif