// Service.h
#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include "../../../con2redis/src/con2redis.h"

class Service {
public:
    Service(int idServer, const std::string& serviceName, long double checkInterval);
    void run();

private:
    int idServer;
    std::string serviceName;
    long double checkInterval;

    void processMessage(redisReply* reply);
    void sendClientIdToOutputStream(const std::string& clientId);
};

#endif // SERVICE_H
