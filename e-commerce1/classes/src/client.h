#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <string>
#include <random>
#include <boost/asio.hpp>

#include "../../../chronos_lib/src/chronoslib.h"
#include "../../../con2redis/src/con2redis.h"


using boost::asio::ip::tcp;

class Client {
public:
    Client(const std::vector<int>& arr, const std::vector<std::string>& reqTypes, const std::vector<std::vector<int>>& matrix, const std::vector<std::string>& servers);
    void run();

private:
    std::vector<int> sleepTimes;
    std::vector<std::string> requestTypes;
    std::vector<std::vector<int>> matrix;
    std::vector<std::string> servers;
    int state;
    tcp::socket* socket;
    std::mt19937 rng;

    void connectToServer();
    void disconnectFromServer();
    void sendRequest();
    int nextState();
};

#endif // CLIENT_H
