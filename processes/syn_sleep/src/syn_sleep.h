#ifndef syn_sleep
#define syn_sleep

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include "../../con2redis/src/con2redis.h"

#define ORCHESTRATOR_STREAM "orchastrator-processes"
#define PROCESS_STREAM "process-orchestrator"
#define REDIS_IP "localhost"
#define REDIS_PORT 6379

int synSleep(int t);

int micro_sleep(long usec);

#endif