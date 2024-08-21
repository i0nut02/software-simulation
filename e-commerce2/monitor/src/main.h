#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../../con2redis/src/con2redis.h"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define MONITOR_STREAM "monitor-monoserver"

#define MONITOR_LEN 1000
#define QUERY_LEN 1000

int micro_sleep(long usec);

#endif