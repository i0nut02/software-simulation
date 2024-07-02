// Server.cpp
#include "server.h"
#include <iostream>
#include <sstream>
#include <boost/bind.hpp>

Server::Server(const std::string& ipAddress, int port, const std::vector<std::string>& msgList,
               long double acceptConnTime, long double readTimeResponse, long double readReqTime, int idServer)
    : ioContext(),
      acceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(ipAddress), port)),
      ipAddress(ipAddress), port(port), msgList(msgList),
      acceptConnTime(acceptConnTime), readTimeResponse(readTimeResponse), readReqTime(readReqTime), idServer(idServer) {}

void Server::run() {
    while (true) {
        acceptConnections();
        checkClientRequests();
        handleRedisStream();
    }
}

void Server::acceptConnections() {
    try {
        boost::asio::ip::tcp::socket socket(ioContext);
        acceptor.accept(socket);
        int clientId = socket.remote_endpoint().port();
        clients[clientId] = std::move(socket);

        std::cout << "Accepted connection from client: " << clientId << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Accept error: " << e.what() << std::endl;
    }
}

void Server::checkClientRequests() {
    for (auto it = clients.begin(); it != clients.end(); ) {
        try {
            readRequest(it->second, it->first);
            ++it;
        } catch (std::exception& e) {
            std::cerr << "Error handling client " << it->first << ": " << e.what() << std::endl;
            it = clients.erase(it); // Remove disconnected client
        }
    }
}

void Server::readRequest(boost::asio::ip::tcp::socket& socket, int clientId) {
    try {
        char data[512];
        boost::system::error_code error;
        
        socket.non_blocking(true);  // Set socket to non-blocking mode
        
        size_t length = socket.read_some(boost::asio::buffer(data), error);
        if (error == boost::asio::error::would_block) {
            // No data to read right now
            return;
        } else if (error == boost::asio::error::eof) {
            // Connection closed cleanly by peer
            std::cout << "Client " << clientId << " disconnected" << std::endl;
            throw std::runtime_error("Client disconnected");
        } else if (error) {
            throw boost::system::system_error(error); // Some other error
        }

        std::string request(data, length);
        std::cout << "Received request from client " << clientId << ": " << request << std::endl;

        std::string endpoint = parseEndpoint(request);
        std::cout << "Requested endpoint: " << endpoint << std::endl;
        // TODO: send to service

    } catch (std::exception& e) {
        std::cerr << "Read error for client " << clientId << ": " << e.what() << std::endl;
        throw; // Rethrow the exception to be caught in checkClientRequests
    }
}

std::string Server::parseEndpoint(const std::string& request) {
    std::istringstream requestStream(request);
    std::string requestLine;
    std::getline(requestStream, requestLine);

    std::istringstream lineStream(requestLine);
    std::string method, endpoint, version;
    lineStream >> method >> endpoint >> version;

    return endpoint;
}

void Server::handleRedisStream() {
    // TODO: implement all about redis here
}

void Server::sendToClient(int clientId, const std::string& message) {
    try {
        auto it = clients.find(clientId);
        if (it != clients.end()) {
            boost::asio::write(it->second, boost::asio::buffer(message + "\n"));
        }
    } catch (std::exception& e) {
        std::cerr << "Send error for client " << clientId << ": " << e.what() << std::endl;
    }
}
