#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>
#include <fcntl.h>
#include <sys/select.h>

#include "../../../chronos_lib/src/chronoslib.h"
#include "../../../con2redis/src/con2redis.h"

#define REDIS_IP "localhost"
#define REDIS_PORT 6379

class Server {
public:
    Server(const std::string& ipAddress, int port, const std::vector<std::string>& msgList,
           long double acceptConnTime, long double readTimeResponse, long double readReqTime, int idServer);

    ~Server();

    void run();

private:
    void acceptConnections();
    bool readRequest(int clientFd); // Returns false if the client should be disconnected
    void handleRedisStream();
    void sendToClient(int clientId, const std::string& message);
    std::string parseEndpoint(const std::string& request);

    int serverFd; // Server socket file descriptor
    std::unordered_set<int> clients;

    redisContext *c2r;
    redisReply *reply;
    
    std::string ipAddress;
    int port;
    std::vector<std::string> services;
    long double acceptConnTime;
    long double readTimeResponse;
    long double readReqTime;
    int idServer;
};

#endif // SERVER_H
