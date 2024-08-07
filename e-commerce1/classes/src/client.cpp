#include "client.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdarg>

// Constructor
Client::Client(const std::vector<long double>& arr, const std::vector<std::string>& reqTypes,
               const std::vector<std::vector<long double>>& matrix, const std::vector<int>& servers,
               const std::string address, int port)
    : sleepTimes(arr), requestTypes(reqTypes), matrix(matrix), servers(servers), state(0), c2r(nullptr), clientId(-1) {
    std::random_device rd;
    rng.seed(rd());

    c2r = redisConnect(address.c_str(), port);
    if (c2r == NULL || c2r->err) {
        if (c2r) {
            std::cerr << "Error: " << c2r->errstr << std::endl;
            redisFree(c2r);
        } else {
            std::cerr << "Can't allocate redis context" << std::endl;
        }
        c2r = nullptr;
    }
    connect();
}

// Destructor
Client::~Client() {
    if (c2r != nullptr) {
        redisFree(c2r);
    }
}

// Main run loop
void Client::run() {
    while (_currentTimestamp < 30*24*60*60) {
        std::uniform_real_distribution<long double> dist(0.0, sleepTimes[state]);

        mySleep(dist(rng));

        if (state == 0) {
            connectToServer();
        } else if (state == requestTypes.size() + 1) {
            disconnectFromServer();
        } else {
            sendRequest();
        }
        state = nextState();
    }
    disconnect();
}

// Determine the next state
int Client::nextState() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double randNum = dist(rng);

    long double cumulativeProbability = 0.0;
    for (int nextState = 0; nextState < matrix[state].size(); ++nextState) {
        cumulativeProbability += matrix[state][nextState];
        if (randNum < cumulativeProbability) {
            return nextState;
        }
    }

    return state; // Should never reach here under normal circumstances
}

// Connect to a server
void Client::connectToServer() {
    if (c2r == nullptr) {
        std::cerr << "Not connected to any server" << std::endl;
        return;
    }
    std::uniform_int_distribution<int> dist(0, servers.size() - 1);
    server = servers[dist(rng)];

    std::string streamName = std::to_string(server) + "-clients";
    initStreams(c2r, streamName.c_str());

    streamName = std::to_string(server)  + "-connections";
    initStreams(c2r, streamName.c_str());

    makeWaitUnlock();
    std::cout << 1 << std::endl;
    redisReply* reply = executeCommand("XADD %d-clients * request connection", server);
    freeReplyObject(reply);
    synSleep(0.01L);

    alertBlocking();
    reply = executeCommand("XREADGROUP GROUP diameter client BLOCK 20000 COUNT 1 STREAMS %d-connections >", server);
    assertReply(c2r, reply);
    unblock();

    if (ReadNumStreams(reply) == 0) {
        synSleep(0.01L);
        return;
    }

    char clientIdChar[INT64_WIDTH];
    memset(clientIdChar, 0, INT64_WIDTH);

    ReadStreamMsgVal(reply, 0, 0, 1, clientIdChar);

    clientId = atoi(clientIdChar);

    streamName = std::to_string(server)  + "-" + std::to_string(clientId);
    initStreams(c2r, streamName.c_str());
    return;
}

// Disconnect from the server
void Client::disconnectFromServer() {
    if (c2r != nullptr) {
        synSleep(0.01L);
        makeWaitUnlock();
        std::cout << 2 << std::endl;
        executeCommand("XADD %d-clients * request disconnection clientId %d", server, clientId);
    }
    redisReply* reply = RedisCommand(c2r, "DEL %d-%d", server, clientId);
    assertReply(c2r, reply);
}

// Send a request to the server
void Client::sendRequest() {
    if (c2r == nullptr) {
        std::cerr << "Not connected to any server" << std::endl;
        return;
    }

    std::string requestType = requestTypes[state - 1];
    synSleep(0.01L);
    makeWaitUnlock();
    std::cout << 3 << std::endl;
    executeCommand("XADD %d-clients * request %s clientId %d timestamp %s", server, requestType.c_str(), clientId, getSimulationTimestamp().c_str());
    synSleep(0.01L);

    alertBlocking();
    executeCommand("XREADGROUP GROUP diameter client BLOCK 20000 COUNT 1 STREAMS %d-%d >", server, clientId);
    unblock();
}

redisReply* Client::executeCommand(const char* format, ...) {
    va_list args;
    va_start(args, format);
    redisReply* reply = (redisReply*)redisvCommand(c2r, format, args);
    va_end(args);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR) {
        std::cerr << "Error executing command: " << (reply ? reply->str : "NULL reply") << std::endl;
        if (reply) freeReplyObject(reply);
        return nullptr;
    }
    return reply;
}
