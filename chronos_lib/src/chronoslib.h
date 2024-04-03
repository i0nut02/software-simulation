#ifndef CHRONS_LIB
#define CHRONOS_LIB

#include <string>
#include <iostream>
#include <cstring>

#include "../../con2redis/src/con2redis.h"

#define VALUE_LEN 5000

extern int pid;
extern redisContext *c2r;
extern redisReply *reply;

int connect(char *redisIP = const_cast<char*>("localhost"), int redisPort = 6379);

void synSleep(long double T);

void alertBlocking();

void mySleep(long double T);

void disconnect();

#endif