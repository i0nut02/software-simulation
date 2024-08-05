#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <hiredis/hiredis.h>
#include "../../../con2redis/src/con2redis.h"  // Include the con2redis header
#include "../../../chronos_lib/src/chronoslib.h"

#define REQUEST_TYPE_LEN 100

#define MONITOR_STREAM "monitor-monoserver"

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

    std::unordered_map<int, std::vector<std::string>> timeMap;

    void processRequest(redisReply* reply);
    int generateClientId();
    void handleConnection(int clientId);
    void handleDisconnection(int clientId);
    void forwardMessage(const std::string& requestType, int clientId);
    void logResponse(int clientId);
    void logEfficienty();
    // Utility method to execute a Redis command and handle errors
    redisReply* executeCommand(const char* format, ...);
};

#endif
