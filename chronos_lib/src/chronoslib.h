#ifndef CHRONS_LIB
#define CHRONOS_LIB

#include <string>
#include <iostream>

#include "time_formatter.h"
#include "../../con2redis/src/con2redis.h"

#define VALUE_LEN 100

extern int pid;
extern redisContext *c2r;
extern redisReply *reply;

int connect(char *redisIP = const_cast<char*>("localhost"), int redisPort = 6379);

void synSleep(TimeFormatter T);

void alertBlocking();

void mySleep(TimeFormatter T);

void disconnect();

#endif