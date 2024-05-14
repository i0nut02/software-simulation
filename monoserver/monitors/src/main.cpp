#include "main.h"

int micro_sleep(long usec) {
    struct timespec ts;
    int res;

    if (usec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = usec / 1000000;
    ts.tv_nsec = (usec % 1000000) * 1000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int main() {
    PGresult *query_res;
    Con2DB db(POSTGRESQL_SERVER, POSTGRESQL_PORT, POSTGRESQL_USER, POSTGRESQL_PSW, POSTGRESQL_DBNAME);

    char query[QUERY_LEN];


    while (true) {
        micro_sleep(1000000);

        sprintf(query, "WITH lastView AS (SELECT COALESCE(MAX(toTime), 0) AS newFrom FROM ResponseTimeMonitors),\n"
                        "    calcTable AS (SELECT l.* FROM Logs l, lastView lv WHERE l.timeResponse > lv.newFrom)\n"
                        "INSERT INTO ResponseTimeMonitors \n"
                        "SELECT lv.newFrom, COALESCE(MAX(ct.timeResponse), lv.newFrom), count(ct.*), COALESCE(MIN(ct.timeResponse - ct.timeRequest), 0), COALESCE(MAX(ct.timeResponse - ct.timeRequest), 0), COALESCE(AVG(ct.timeResponse - ct.timeRequest), 0)\n"
                        "FROM calcTable ct, lastView lv\n"
                        "GROUP BY lv.newFrom;"
                        );
        query_res = db.ExecSQLcmd(query);

        if (PQresultStatus(query_res) != PGRES_COMMAND_OK) {
            std::cout << "Error during Monitors Query" << std::endl;
            return 1;
        }
    }
    return 0;
}