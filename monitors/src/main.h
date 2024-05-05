#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <string>

#include "../../con2db/pgsql.h"

#define POSTGRESQL_SERVER "localhost"
#define POSTGRESQL_PORT "5432"
#define POSTGRESQL_USER "admin"
#define POSTGRESQL_PSW "admin"
#define POSTGRESQL_DBNAME "log_db"

#define QUERY_LEN 1000

int micro_sleep(long usec);

#endif