#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <string>
#include <random>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

class Client {
public:
    // Constructor with necessary parameters
    Client(const std::vector<int>& arr, const std::vector<std::string>& reqTypes, 
           const std::vector<std::vector<long double>>& matrix, const std::vector<std::string>& servers);
    
    // Main function to run the client logic
    void run();

private:
    std::vector<int> sleepTimes;                      // Sleep times for each state
    std::vector<std::string> requestTypes;            // List of request types
    std::vector<std::vector<long double>> matrix;     // State transition matrix
    std::vector<std::string> servers;                 // List of servers in address:port format
    int state;                                        // Current state of the client
    int sockfd;                                       // Socket file descriptor
    std::mt19937 rng;                                 // Random number generator

    // Connect to a server
    void connectToServer();
    
    // Disconnect from the server
    void disconnectFromServer();
    
    // Send a request to the server
    void sendRequest();
    
    // Determine the next state based on transition probabilities
    int nextState();
};

#endif // CLIENT_H
