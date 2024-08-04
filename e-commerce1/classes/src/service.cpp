// Service.cpp
#include "service.h"
#include <iostream>
#include <chrono>
#include <cstring>

void Service::run() {
    redisContext* c2r = redisConnect(REDIS_IP, REDIS_PORT);
    redisReply* reply;
    
    if (c2r == nullptr || c2r->err) {
        if (c2r) {
            std::cerr << "Connection error: " << c2r->errstr << std::endl;
            redisFree(c2r);
        } else {
            std::cerr << "Connection error: can't allocate redis context" << std::endl;
        }
        return;
    }

    std::string streamName = serviceName + "-" + std::to_string(idServer);
    initStreams(c2r, streamName.c_str());

    streamName = std::to_string(idServer) + "-clients";
    initStreams(c2r, streamName.c_str());

    char clientId[INT64_WIDTH];
    char typeRequest[REQ_LEN];

    int requestCount = 0; // Counter for processed requests

    connect();

    while (_currentTimestamp < 30*24*60*60) {
        alertBlocking();
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter service BLOCK 20000 COUNT 1 STREAMS %s-%d >", serviceName.c_str(), idServer);
        unblock();

        if (ReadNumStreams(reply) == 0) {
            synSleep(0.01L);
            continue;
        }

        // Extracting the ID from the reply
        char lastId[128];
        memset(lastId, 0, 128);
        ReadStreamNumMsgID(reply, 0, 0, lastId);

        memset(clientId, 0, INT64_WIDTH);
        memset(typeRequest, 0, REQ_LEN);

        ReadStreamMsgVal(reply, 0, 0, 1, clientId);
        ReadStreamMsgVal(reply, 0, 0, 3, typeRequest);

        freeReplyObject(reply);
        
        for (size_t k = 0; k < services.size(); ++k) {
            if (services[k] == typeRequest) {
                // Process the request
                synSleep(times[k]);
                reply = RedisCommand(c2r, "XADD %d-clients * type response clientId %s", idServer, clientId);
                freeReplyObject(reply);
                break;
            }
        }

        requestCount++; // Increment the request counter

        // Trim the stream every 500 processed requests
        if (requestCount >= 500) {
            reply = RedisCommand(c2r, "XTRIM %s-%d MINID %s", serviceName.c_str(), idServer, lastId);
            freeReplyObject(reply);
            requestCount = 0; // Reset the counter
        }
    }

    redisFree(c2r);
    disconnect();
}
