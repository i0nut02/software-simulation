// Client.cpp
#include "client.h"

// Constructor
Client::Client(const std::vector<int>& arr, const std::vector<std::string>& reqTypes, 
               const std::vector<std::vector<long double>>& matrix, const std::vector<std::string>& servers)
    : sleepTimes(arr), requestTypes(reqTypes), matrix(matrix), servers(servers), state(0), sockfd(-1) {
    std::random_device rd;
    rng.seed(rd());
}

// Main run loop
void Client::run() {
    std::cout << "Starting client loop" << std::endl;
    while (true) {
        std::cout << state << std::endl;
        if (state == 0) {
            connectToServer();
        } else if (state == requestTypes.size() + 1) {
            disconnectFromServer();
        } else {
            sendRequest();
        }
        state = nextState();
    }
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
    std::uniform_int_distribution<int> dist(0, servers.size() - 1);
    std::string server = servers[dist(rng)];
    std::cout << "Connecting to server: " << server << std::endl;

    // Parse server string to extract address and port
    std::string address;
    std::string port;
    size_t colonPos = server.find(':');
    if (colonPos != std::string::npos) {
        address = server.substr(0, colonPos);
        port = server.substr(colonPos + 1);
    } else {
        std::cerr << "Invalid server format. Use address:port" << std::endl;
        return;
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (getaddrinfo(address.c_str(), port.c_str(), &hints, &res) != 0) {
        perror("getaddrinfo failed");
        return;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("Socket creation failed");
        freeaddrinfo(res);
        return;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Connection failed");
        close(sockfd);
        sockfd = -1;
    } else {
        std::cout << "Connected to " << server << std::endl;
    }

    freeaddrinfo(res);
}

// Disconnect from the server
void Client::disconnectFromServer() {
    if (sockfd != -1) {
        std::cout << "Disconnecting from server" << std::endl;
        close(sockfd);
        sockfd = -1;
    }
}

// Send a request to the server
void Client::sendRequest() {
    if (sockfd == -1) {
        std::cerr << "Not connected to any server" << std::endl;
        return;
    }

    std::string request = "GET " + requestTypes[state-1] + " HTTP/1.1\r\n";
    request += "Host: localhost\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";

    std::cout << "Sending request: " << requestTypes[state-1] << std::endl;

    ssize_t bytesSent = send(sockfd, request.c_str(), request.length(), 0);
    if (bytesSent < 0) {
        perror("Send failed");
        disconnectFromServer();
        return;
    }

    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        std::cout << "Cazzi: " << buffer << std::endl;
        perror("Receive failed");
        disconnectFromServer();
    } else {
        std::cout << "Response: " << buffer << std::endl;
    }
}
