// Service.h
#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <vector>

#include "../../../con2redis/src/con2redis.h"
#include "../../../chronos_lib/src/chronoslib.h"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

#define REQ_LEN 100

class Service {
public:
    Service(int idServer, const std::string& serviceName, const std::vector<std::string>& services, const std::vector<long double>& times)
        : idServer(idServer), serviceName(serviceName), services(services), times(times) {}
    void run();

private:
    int idServer;
    std::string serviceName;
    std::vector<std::string> services;
    std::vector<long double> times;

    void processMessage(redisReply* reply);
    void sendClientIdToOutputStream(const std::string& clientId);
};

#endif // SERVICE_H
