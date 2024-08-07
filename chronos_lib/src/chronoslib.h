#ifndef CHRONOSLIB
#define CHRONOSLIB

#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>


#include "../../con2redis/src/con2redis.h"

#define REQUEST_CONNECTION "request-connection"
#define IDS_CONNECTION "ids-connection"
#define SEND_STREAM "orchestrator-in"
#define LOGGER_STREAM "chronos-logger"

#define CONN_REQ "Connection request"
#define WAIT_ID "Waiting ID"
#define BLOCKING_CALL "Blocking Call"
#define UNBLOCK "Unblocked"
#define SYN_SLEEP "Sync sleep"
#define MY_SLEEP "My sleep"
#define WAIT_SYNC "Wait sync"
#define DISCONNECT "Disconnection"

#define VALUE_LEN 5000
#define KEY_LEN 100
#define ORCHERTRATOR_ID 0
#define NULL_PARAM -1

extern int _pid;
extern int _logLvl;
extern long double _currentTimestamp;
extern long double _efficienty;

extern redisContext *_c2r;
extern redisReply *_reply;

int connect(char *redisIP = const_cast<char*>("localhost"), int redisPort = 6379, int logLvl = 3);

void alertBlocking();

void unblock();

void makeWaitUnlock();

void synSleep(long double T);

void mySleep(long double T);

void disconnect();

void logRedis(const char *stream, const char *message, long double value);

std::string getSimulationTimestamp();

std::string getCurrentTime();

#endif