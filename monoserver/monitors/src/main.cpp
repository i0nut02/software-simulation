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
    redisReply* reply;
    redisContext* c2r;
    char lastMsgID[64] = "0-0";

    char valueStart[MONITOR_LEN], valueEnd[MONITOR_LEN];
    long double ldStart, ldEnd;

    long double timeResponse, maxTimeResponse = -std::numeric_limits<long double>::infinity();
    long double minTimeResponse = std::numeric_limits<long double>::infinity();
    long double sumTimeResponses = 0.0;

    int numResponses = 0;

    std::ofstream file("../log_monitors.txt", std::ios_base::app);
    std::ofstream monitoring("../live_monitoring.txt", std::ios_base::app);

    if (!file) {
        std::cerr << "Error opening log file." << std::endl;
        return 0;
    }
    if (!monitoring) {
        std::cerr << "Error opening log file." << std::endl;
        return 0;
    }

    file << "startRequest;endResponse" << std::endl;
    monitoring << "min;max;mean" << std::endl;

    c2r = redisConnect(REDIS_IP, REDIS_PORT);

    initStreams(c2r, MONITOR_STREAM);

    while (1) {
        int i;
        for (i =0; i < 100; i++) {
            reply = RedisCommand(c2r, "XREADGROUP GROUP diameter orchestrator BLOCK 10000 COUNT 1 STREAMS %s >", MONITOR_STREAM);
            assertReply(c2r, reply);

            if (ReadNumStreams(reply) == 0) {
                std::cout << "No monitor log registered" << std::endl;
                break;
            }

            memset(valueStart, 0, MONITOR_LEN);
            memset(valueEnd, 0, MONITOR_LEN);

            ReadStreamMsgVal(reply, 0, 0, 1, valueStart);
            ReadStreamMsgVal(reply, 0, 0, 3, valueEnd);
            
            ldStart = strtold(valueStart, NULL);
            ldEnd = strtold(valueEnd, NULL);

            file << ldStart << ";"<< ldEnd << std::endl;

            timeResponse = ldEnd - ldStart;

            numResponses++;
            maxTimeResponse = std::max(maxTimeResponse, timeResponse);
            minTimeResponse = std::min(minTimeResponse, timeResponse);
            sumTimeResponses += timeResponse;

            ReadStreamNumMsgID(reply, 0, 0, lastMsgID);
            freeReplyObject(reply);
        }

        // stop the monitor
        if (i == 0) {
            break;
        }

        monitoring << minTimeResponse << ";" << maxTimeResponse << ";" << sumTimeResponses / numResponses << std::endl;

        void* trimReply = RedisCommand(c2r, "XTRIM %s MINID %s", MONITOR_STREAM, lastMsgID);
        freeReplyObject(trimReply);
    }

    file.close();
    monitoring.close();

    reply = RedisCommand(c2r, "DEL %s", MONITOR_STREAM);
    assertReply(c2r, reply);

    return 0;
}