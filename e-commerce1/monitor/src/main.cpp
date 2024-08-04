#include "main.h"
#include <iostream>
#include <random>
#include <limits>
#include <cstring>
#include <cmath>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

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

char* processTimeLog(redisReply* reply, long double* ldStart, long double* ldRead, long double* ldEnd) {
    char serverId[MONITOR_LEN], reqType[MONITOR_LEN], start[MONITOR_LEN], read[MONITOR_LEN], end[MONITOR_LEN];
    char* result = (char*)malloc(MONITOR_LEN * 5 + 5);

    memset(serverId, 0, MONITOR_LEN);
    memset(reqType, 0, MONITOR_LEN);
    memset(start, 0, MONITOR_LEN);
    memset(read, 0, MONITOR_LEN);
    memset(end, 0, MONITOR_LEN);
    memset(result, 0, MONITOR_LEN * 5 + 5);

    ReadStreamMsgVal(reply, 0, 0, 3, serverId);
    ReadStreamMsgVal(reply, 0, 0, 5, reqType);
    ReadStreamMsgVal(reply, 0, 0, 7, start);
    ReadStreamMsgVal(reply, 0, 0, 9, read);
    ReadStreamMsgVal(reply, 0, 0, 11, end);

    sprintf(result, "%s;%s;%s;%s;%s", serverId, reqType, start, read, end);

    *ldStart = strtold(start, NULL);
    *ldRead = strtold(read, NULL);
    *ldEnd = strtold(end, NULL);

    return result;
}

char* processEfficiencyLog(redisReply* reply) {
    char id[MONITOR_LEN], efficiency[MONITOR_LEN];
    char* result = (char*)malloc(MONITOR_LEN * 2 + 2);

    memset(id, 0, MONITOR_LEN);
    memset(efficiency, 0, MONITOR_LEN);
    memset(result, 0, MONITOR_LEN * 2 + 2);

    ReadStreamMsgVal(reply, 0, 0, 3, id);
    ReadStreamMsgVal(reply, 0, 0, 5, efficiency);

    sprintf(result, "%s;%s", id, efficiency);

    return result;
}

int main(int argc, char* argv[]) {
    redisReply* reply;
    redisContext* c2r;
    char lastMsgID[64] = "0-0";

    char type[MONITOR_LEN];

    long double ldStart = 0.0, ldEnd = 0.0, ldRead = 0.0;

    long double timeResponse = 0.0, maxTimeResponse = std::numeric_limits<long double>::lowest();
    long double minTimeResponse = std::numeric_limits<long double>::max();
    long double maxQueueTime = std::numeric_limits<long double>::lowest();
    long double minQueueTime = std::numeric_limits<long double>::max();
    long double sumTimeResponses = 0.0, sumQueueTimes = 0.0;

    int numResponses = 0;
    
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " n1 n2 n3 n4 n5 n6" << std::endl;
        return 1;
    }

    // Parse input numbers
    int n[6];
    for (int i = 0; i < 6; i++) {
        n[i] = std::stoi(argv[i + 1]);
    }

    // Construct directory path
    std::stringstream ss;
    ss << "../" << n[0] << "-" << n[1] << "-" << n[2] << "-" << n[3] << "-" << n[4] << "-" << n[5];
    std::string dirPath = ss.str();

    // Create directory
    struct stat info;
    if (stat(dirPath.c_str(), &info) != 0) {
        // Directory does not exist
        if (mkdir(dirPath.c_str(), 0777) != 0) {
            std::cerr << "Error creating directory " << dirPath << std::endl;
            return 1;
        }
    } else if (!(info.st_mode & S_IFDIR)) {
        std::cerr << dirPath << " is not a directory!" << std::endl;
        return 1;
    }

    // Open files in the created directory
    std::ofstream file(dirPath + "/log_monitors.txt", std::ios_base::trunc);
    std::ofstream monitoring(dirPath + "/live_monitoring.txt", std::ios_base::trunc);
    std::ofstream queueMonitoring(dirPath + "/queue_time_monitoring.txt", std::ios_base::trunc);
    std::ofstream efficiency(dirPath + "/efficiency.txt", std::ios_base::trunc);

    if (!file || !monitoring || !efficiency || !queueMonitoring) {
        std::cerr << "Error opening files." << std::endl;
        return 0;
    }

    file << "server id;request type;request time;read request;response time" << std::endl;
    monitoring << "requests;min;max;mean" << std::endl;
    queueMonitoring << "requests;min;max;mean" << std::endl;
    efficiency << "id;efficiency" << std::endl;

    c2r = redisConnect(REDIS_IP, REDIS_PORT);

    initStreams(c2r, MONITOR_STREAM);

    while (1) {
        int i;
        for (i = 0; i < 100; i++) {
            reply = RedisCommand(c2r, "XREADGROUP GROUP diameter monitor BLOCK 25000 COUNT 1 STREAMS %s >", MONITOR_STREAM);
            assertReply(c2r, reply);

            if (ReadNumStreams(reply) == 0) {
                std::cout << "No monitor log registered" << std::endl;
                break;
            }

            memset(type, 0, MONITOR_LEN);

            ReadStreamMsgVal(reply, 0, 0, 1, type);

            if (strcmp(type, "time") == 0) {
                char* timeLog = processTimeLog(reply, &ldStart, &ldRead, &ldEnd);
                file << timeLog << std::endl;

                timeResponse = ldEnd - ldStart;

                numResponses++;
                maxTimeResponse = std::max(maxTimeResponse, timeResponse);
                minTimeResponse = std::min(minTimeResponse, timeResponse);

                maxQueueTime = std::max(maxQueueTime, ldRead - ldStart);
                minQueueTime = std::min(minQueueTime, ldRead - ldStart);

                sumTimeResponses += timeResponse;
                sumQueueTimes += ldRead - ldStart;
            } else {
                char* efficiencyLog = processEfficiencyLog(reply);
                efficiency << efficiencyLog << std::endl;
            }

            ReadStreamNumMsgID(reply, 0, 0, lastMsgID);
            freeReplyObject(reply);
        }

        // stop the monitor
        if (i == 0) {
            break;
        }

        if (numResponses > 0) {
            monitoring << numResponses << ";"
                       << minTimeResponse << ";"
                       << maxTimeResponse << ";"
                       << (numResponses != 0 ? sumTimeResponses / numResponses : 0) << std::endl;
            queueMonitoring << numResponses << ";"
                            << minQueueTime << ";"
                            << maxQueueTime << ";"
                            << (numResponses != 0 ? sumQueueTimes / numResponses : 0) << std::endl;

            numResponses = 0;
            sumTimeResponses = 0.0;
            sumQueueTimes = 0.0;
            maxTimeResponse = std::numeric_limits<long double>::lowest();
            minTimeResponse = std::numeric_limits<long double>::max();
            maxQueueTime = std::numeric_limits<long double>::lowest();
            minQueueTime = std::numeric_limits<long double>::max();
        }

        void* trimReply = RedisCommand(c2r, "XTRIM %s MINID %s", MONITOR_STREAM, lastMsgID);
        freeReplyObject(trimReply);
    }

    file.close();
    monitoring.close();
    queueMonitoring.close();
    efficiency.close();

    reply = RedisCommand(c2r, "DEL %s", MONITOR_STREAM);
    assertReply(c2r, reply);

    return 0;
}
