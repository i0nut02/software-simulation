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

    connect();
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
    synSleep(0.1L);
    makeWaitUnlock();
    std::cout << 4 << std::endl;
    redisReply* reply = RedisCommand(c2r, "XADD %d-connections * clientId %d", idServer, clientId);
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);
    //synSleep(0.01L);
    
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
        synSleep(0.02L);
        makeWaitUnlock();
        std::cout << 5 << std::endl;
        redisReply* reply = RedisCommand(c2r, "XADD %s-%d * clientId %d request %s", serviceMap[requestType].c_str(), idServer, clientId, requestType.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        //synSleep(0.01L);
    }
    return;
}

void Server::processRequest(redisReply* reply) {
    char clientIdChar[INT64_WIDTH];
    char requestType[REQUEST_TYPE_LEN];
    char timeRequest[REQUEST_TYPE_LEN];

    memset(clientIdChar, 0, INT64_WIDTH);
    memset(requestType, 0, REQUEST_TYPE_LEN);
    memset(timeRequest, 0, REQUEST_TYPE_LEN);

    ReadStreamMsgVal(reply, 0, 0, 1, requestType);

    if (strcmp(requestType, "connection") == 0) {
        handleConnection(generateClientId());
        return;
    }

    ReadStreamMsgVal(reply, 0, 0, 3, clientIdChar);

    if (strcmp(requestType, "disconnection") == 0) {
        handleDisconnection(atoi(clientIdChar));

    } else if (strcmp(requestType, "response") == 0){
        makeWaitUnlock();
        std::cout << 6 << std::endl;
        reply = RedisCommand(c2r, "XADD %d-%s * clientId response", idServer, clientIdChar);
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        logResponse(atoi(clientIdChar));
        //synSleep(0.01L);
    } else {
        ReadStreamMsgVal(reply, 0, 0, 5, timeRequest);
        timeMap[atoi(clientIdChar)] = {timeRequest, getSimulationTimestamp(), requestType};
        forwardMessage(std::string(requestType), atoi(clientIdChar));
    }
}

void Server::run() {
    int requestCount = 0; // Counter for processed requests
    char lastIdClients[128];

    while (_currentTimestamp < 30*24*60*60) {
        alertBlocking();
        redisReply* reply = RedisCommand(c2r, "XREADGROUP GROUP diameter orchestrator BLOCK 20000 COUNT 1 STREAMS %d-clients >", idServer);
        unblock();

        if (!reply || ReadNumStreams(reply) == 0) {
            freeReplyObject(reply);
            synSleep(0.01L);
        } else {
            memset(lastIdClients, 0, 128);
            ReadStreamNumMsgID(reply, 0, 0, lastIdClients);

            processRequest(reply);
            freeReplyObject(reply);
            synSleep(0.1L);
            requestCount++;
        }

        if (requestCount >= 1000) {
            reply = RedisCommand(c2r, "XTRIM %d-clients MINID %s", idServer, lastIdClients);
            requestCount = 0;
        }
    }
    logEfficienty();
    disconnect();
}

void Server::logResponse(int clientId) {
    redisReply* reply;
    reply = RedisCommand(c2r, "XADD %s * type time serverId %d reqType %s start %s read %s end %s", MONITOR_STREAM, idServer, timeMap[clientId][2].c_str(), timeMap[clientId][0].c_str(), timeMap[clientId][1].c_str(), getSimulationTimestamp().c_str());
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);
}

void Server::logEfficienty() {
    redisReply* reply;

    reply = RedisCommand(c2r, "XADD %s * type efficiency id %d value %s", MONITOR_STREAM, idServer, std::to_string(_efficienty).c_str());
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);
}