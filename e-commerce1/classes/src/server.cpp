#include "server.h"

#include <ctime>
#include <cstdarg>
#include <cstring>
#include <iostream>

Server::Server(const std::string& redisIP, int redisPort, int idServer, const std::unordered_map<std::string, std::string>& hashtable)
    : idServer(idServer), serviceMap(hashtable) {

    c2r = redisConnect(redisIP.c_str(), redisPort);
    if (c2r == NULL || c2r->err) {
        if (c2r) {
            std::cerr << "Error: " << c2r->errstr << std::endl;
            redisFree(c2r);
        } else {
            std::cerr << "Can't allocate redis context" << std::endl;
        }
        exit(1);
    }
    
    redisReply* reply = RedisCommand(c2r, "DEL %d-connections", idServer);
    assertReply(c2r, reply);

    std::string streamName = std::to_string(idServer) + "-connections";

    initStreams(c2r, streamName.c_str());

    reply = RedisCommand(c2r, "DEL %d-clients", idServer);
    assertReply(c2r, reply);

    streamName = std::to_string(idServer) + "-clients";

    initStreams(c2r, streamName.c_str());
}

Server::~Server() {
    if (c2r) {
        redisFree(c2r);
    }
}

redisReply* Server::executeCommand(const char* format, ...) {
    va_list args;
    va_start(args, format);
    redisReply* reply = RedisCommand(c2r, format, args);
    va_end(args);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
        std::cerr << "Error executing command: " << (reply ? reply->str : "NULL reply") << std::endl;
        if (reply) freeReplyObject(reply);
        return nullptr;
    }
    return reply;
}

int Server::generateClientId() {
    return clientIdCounter++;
}

void Server::handleConnection(int clientId) {
    redisReply* reply = RedisCommand(c2r, "XADD %d-connections * clientId %d", idServer, clientId);
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);
    
    std::string clientStream = std::to_string(idServer) + "-" + std::to_string(clientId);
    initStreams(c2r, clientStream.c_str());
}

void Server::handleDisconnection(int clientId) {
    // Handle disconnection logic here
    executeCommand("DEL %d-%d", idServer, clientId);
    return;
}

void Server::forwardMessage(const std::string& requestType, int clientId) {
    if (serviceMap.count(requestType) > 0) {
        redisReply* reply = RedisCommand(c2r, "XADD %s-%d * clientId %d request %s", serviceMap[requestType].c_str(), idServer, clientId, requestType.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
    }
    return;
}

void Server::processRequest(redisReply* reply) {
    char clientIdChar[INT64_WIDTH];
    char requestType[REQUEST_TYPE_LEN];

    memset(clientIdChar, 0, INT64_WIDTH);
    memset(requestType, 0, REQUEST_TYPE_LEN);

    ReadStreamMsgVal(reply, 0, 0, 1, requestType);

    if (strcmp(requestType, "connection") == 0) {
        freeReplyObject(reply);
        handleConnection(generateClientId());
        return;
    }

    ReadStreamMsgVal(reply, 0, 0, 3, clientIdChar);

    if (strcmp(requestType, "disconnection") == 0) {
        handleDisconnection(atoi(clientIdChar));

    } else if (strcmp(requestType, "response") == 0){
        reply = RedisCommand(c2r, "XADD %d-%s * clientId response", idServer, clientIdChar);
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);

    } else {
        forwardMessage(std::string(requestType), atoi(clientIdChar));
    }
}

void Server::run() {
    int requestCount = 0; // Counter for processed requests
    char lastIdClients[128];

    while (true) {
        redisReply* reply = RedisCommand(c2r, "XREADGROUP GROUP diameter orchestrator BLOCK 0 COUNT 1 STREAMS %d-clients >", idServer);
        
        if (!reply || reply->elements == 0) {
            if (reply) freeReplyObject(reply);
        } else {
            if (ReadNumStreams(reply) != 0) {
                memset(lastIdClients, 0, 128);
                ReadStreamNumMsgID(reply, 0, 0, lastIdClients);

                processRequest(reply);
                requestCount++;
            }
        }

        if (requestCount >= 1000) {
            reply = RedisCommand(c2r, "XTRIM %d-clients MINID %s", idServer, lastIdClients);
            requestCount = 0;
        }
    }
}