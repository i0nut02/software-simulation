#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <hiredis/hiredis.h>
#include "../../../con2redis/src/con2redis.h"  // Include the con2redis header

#define REQUEST_TYPE_LEN 100

class Server {
public:
    Server(const std::string& redisIP, int redisPort, int idServer, const std::unordered_map<std::string, std::string>& hashtable);
    ~Server();

    void run();

private:
    redisContext* c2r;

    int idServer;
    std::string streamName;
    std::unordered_map<std::string, std::string> serviceMap;
    int clientIdCounter;

    void processRequest(redisReply* reply);
    int generateClientId();
    void handleConnection(int clientId);
    void handleDisconnection(int clientId);
    void forwardMessage(const std::string& requestType, int clientId);

    // Utility method to execute a Redis command and handle errors
    redisReply* executeCommand(const char* format, ...);
};

#endif
