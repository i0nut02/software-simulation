// Service.cpp
#include "service.h"
#include <iostream>
#include <chrono>
#include <cstring>

Service::Service(int idServer, const std::string& serviceName, long double checkInterval)
    : idServer(idServer), serviceName(serviceName), checkInterval(checkInterval) {}

void Service::run() {
    redisContext* context = redisConnect("127.0.0.1", 6379);
    if (context == nullptr || context->err) {
        std::cerr << "Redis connection error: " << (context ? context->errstr : "Cannot allocate Redis context") << std::endl;
        return;
    }

    std::string streamName = std::to_string(idServer) + "-" + serviceName;
    std::string lastId = "0";

    while (true) {
        redisReply* reply = (redisReply*)redisCommand(context, "XREAD COUNT 1 STREAMS %s %s", streamName.c_str(), lastId.c_str());

        if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
            char streamId[64];
            ReadStreamNumMsgID(reply, 0, 0, streamId);
            lastId = streamId;

            processMessage(reply);
        }

        if (reply) {
            freeReplyObject(reply);
        }
    }

    redisFree(context);
}

void Service::processMessage(redisReply* reply) {
    char clientId[256];
    ReadStreamMsgVal(reply, 0, 0, 1, clientId);
    std::cout << "Processing client ID: " << clientId << std::endl;

    sendClientIdToOutputStream(clientId);
}

void Service::sendClientIdToOutputStream(const std::string& clientId) {
    redisContext* context = redisConnect("127.0.0.1", 6379);
    if (context == nullptr || context->err) {
        std::cerr << "Redis connection error: " << (context ? context->errstr : "Cannot allocate Redis context") << std::endl;
        return;
    }

    std::string outputStream = std::to_string(idServer) + "-out";
    std::string command = "XADD " + outputStream + " * clientId " + clientId;
    redisReply* reply = (redisReply*)redisCommand(context, command.c_str());

    if (reply) {
        freeReplyObject(reply);
    }

    redisFree(context);
}
