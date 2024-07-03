// Service.cpp
#include "service.h"
#include <iostream>
#include <chrono>
#include <cstring>

Service::Service(int idServer, const std::string& serviceName, long double TimeToResponse)
    : idServer(idServer), serviceName(serviceName), TimeToResponse(TimeToResponse) {}

void Service::run() {
    redisContext* c2r = redisConnect(REDIS_IP, REDIS_PORT);
    redisReply* reply;

    std::string streamName = std::to_string(idServer) + "-in";
    initStreams(c2r, streamName.c_str());

    streamName = serviceName + "-" + std::to_string(idServer);
    initStreams(c2r, streamName.c_str());

    std::string lastId = "0";
    char clientId[INT64_WIDTH];

    while (true) {
        reply = RedisCommand(c2r, "XREADGROUP diameter service BLOCK 10000 COUNT 1 STREAMS %s-%d", serviceName.c_str(), idServer);

        if (ReadNumStreams(reply) == 0) {
            // XTRIM
            continue;
        }

        memset(clientId, 0, INT64_WIDTH);
        ReadStreamMsgVal(reply, 0, 0, 1, clientId);
        freeReplyObject(reply);

        synSleep(TimeToResponse);

        reply = RedisCommand(c2r, "XADD %d-in * clientId %s", idServer, clientId);
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
    }
}
