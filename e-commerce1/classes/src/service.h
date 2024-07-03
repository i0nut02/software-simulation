// Service.h
#ifndef SERVICE_H
#define SERVICE_H

#include <string>

#include "../../../con2redis/src/con2redis.h"
#include "../../../chronos_lib/src/chronoslib.h"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

class Service {
public:
    Service(int idServer, const std::string& serviceName, long double checkInterval);
    void run();

private:
    int idServer;
    std::string serviceName;
    long double TimeToResponse;

    void processMessage(redisReply* reply);
    void sendClientIdToOutputStream(const std::string& clientId);
};

#endif // SERVICE_H
