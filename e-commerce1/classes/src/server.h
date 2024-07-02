#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../../chronos_lib/src/chronoslib.h"
#include "../../../con2redis/src/con2redis.h"

class Server {
public:
    Server(const std::string& ipAddress, int port, const std::vector<std::string>& msgList,
           long double acceptConnTime, long double readTimeResponse, long double readReqTime, int idServer);

    void run();

private:
    void acceptConnections();
    void checkClientRequests();
    void readRequest(boost::asio::ip::tcp::socket& socket, int clientId);
    void handleRedisStream();
    void sendToClient(int clientId, const std::string& message);
    std::string parseEndpoint(const std::string& request);

    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::acceptor acceptor;
    std::unordered_map<int, boost::asio::ip::tcp::socket> clients;
    
    std::string ipAddress;
    int port;
    std::vector<std::string> msgList;
    long double acceptConnTime;
    long double readTimeResponse;
    long double readReqTime;
    int idServer;
};

#endif // SERVER_H
