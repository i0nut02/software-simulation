#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <string>
#include <random>
#include <unordered_map>

#include "../../../con2redis/src/con2redis.h"
#include "../../../chronos_lib/src/chronoslib.h"


class Client {
public:
    Client(const std::vector<long double>& arr, const std::vector<std::string>& reqTypes,
           const std::vector<std::vector<long double>>& matrix, const std::vector<int>& servers,
           const std::string address, int port, int limitRequests);
    ~Client();
    void run();

private:
    std::vector<long double> sleepTimes;
    std::vector<std::string> requestTypes;
    std::vector<std::vector<long double>> matrix;
    std::vector<int> servers;
    int state;
    redisContext* c2r;
    int clientId;
    std::default_random_engine rng;
    int server;

    int numRequests = 0;
    int limitRequests;

    int nextState();
    void connectToServer();
    void disconnectFromServer();
    void sendRequest();
    redisReply* executeCommand(const char* format, ...);
};

#endif
