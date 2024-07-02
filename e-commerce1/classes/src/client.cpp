// Client.cpp
#include "client.h"
#include <iostream>

Client::Client(const std::vector<int>& arr, const std::vector<std::string>& reqTypes, const std::vector<std::vector<int>>& matrix, const std::vector<std::string>& servers)
    : sleepTimes(arr), requestTypes(reqTypes), matrix(matrix), servers(servers), state(0), socket(nullptr) {
    std::random_device rd;
    rng.seed(rd());
}

void Client::run() {
    connect();
    while (true) {
        mySleep(sleepTimes[state]);

        if (state == 0) {
            connectToServer();
        } else if (state == requestTypes.size() - 1) {
            disconnectFromServer();
        } else {
            sendRequest();
        }
        state = nextState();
    }
    disconnect();
}

int Client::nextState() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double randNum = dist(rng);

    // Determine the next state based on transition probabilities
    double cumulativeProbability = 0.0;
    for (int nextState = 0; nextState < matrix[state].size(); ++nextState) {
        cumulativeProbability += matrix[state][nextState];
        if (randNum < cumulativeProbability) {
            return nextState;
        }
    }

    // Should never reach here under normal circumstances
    return state; // Default to staying in the current state
}

void Client::connectToServer() {
    std::uniform_int_distribution<int> dist(0, servers.size() - 1);
    std::string server = servers[dist(rng)];
    std::cout << "Connecting to server: " << server << std::endl;

    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(server, "http");
        socket = new tcp::socket(io_context);
        boost::asio::connect(*socket, endpoints);
        std::cout << "Connected to " << server << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        socket = nullptr;
    }
}

void Client::disconnectFromServer() {
    if (socket) {
        std::cout << "Disconnecting from server" << std::endl;
        socket->close();
        delete socket;
        socket = nullptr;
    }
}

void Client::sendRequest() {
    if (!socket) {
        std::cerr << "Not connected to any server" << std::endl;
        return;
    }

    std::string request = "GET " + requestTypes[state] + " HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "Connection: close\r\n";
    request += "\r\n"; // End of headers

    std::cout << "Sending request: " << requestTypes[state] << std::endl;

    try {
        boost::asio::write(*socket, boost::asio::buffer(request));

        boost::asio::streambuf response;

        alertBlocking();
        boost::asio::read_until(*socket, response, "\n");
        unblock();

        std::istream response_stream(&response);
        std::string response_str;
        std::getline(response_stream, response_str);
        std::cout << "Response: " << response_str << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Request failed: " << e.what() << std::endl;
    }
}
