#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <string>
#include <random>

#include "../../../chronos_lib/src/chronoslib.h"
#include "../../../con2redis/src/con2redis.h"
#include "../../../con2db/pgsql.h"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define SECOND 1
#define ONEDAY SECOND * 60 * 60 * 24
#define LAST 30 * ONEDAY

#define POSTGRESQL_SERVER "localhost"
#define POSTGRESQL_PORT "5432"
#define POSTGRESQL_USER "admin"
#define POSTGRESQL_PSW "admin"
#define POSTGRESQL_DBNAME "log_db"

#endif