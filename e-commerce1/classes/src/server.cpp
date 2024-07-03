// Server.cpp
#include "server.h"

Server::Server(const std::string& ipAddress, int port, const std::vector<std::string>& services,
               long double acceptConnTime, long double readTimeResponse, long double readReqTime, int idServer)
    : ipAddress(ipAddress), port(port), services(services),
      acceptConnTime(acceptConnTime), readTimeResponse(readTimeResponse), readReqTime(readReqTime), idServer(idServer) {

    // Initialize socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server socket to non-blocking
    if (fcntl(serverFd, F_SETFL, fcntl(serverFd, F_GETFL, 0) | O_NONBLOCK) < 0) {
        perror("Failed to set non-blocking mode");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Set server address
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    serverAddr.sin_port = htons(port);

    // Bind the socket
    if (bind(serverFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverFd, SOMAXCONN) < 0) {
        perror("Listen failed");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    c2r = redisConnect(REDIS_IP, REDIS_PORT);

    std::string stream;
    for (const auto &service : services) {
        stream = service + "-" + std::to_string(idServer);
        initStreams(c2r, stream.c_str());
    }

    stream = std::to_string(idServer) + "-in";
    initStreams(c2r, stream.c_str());
}

void Server::run() {
    std::cout << "starting" << std::endl;

    fd_set readFds;
    int maxFd = serverFd;

    while (true) {
        FD_ZERO(&readFds);

        FD_SET(serverFd, &readFds);

        for (int clientFd : clients) {
            FD_SET(clientFd, &readFds);
            maxFd = std::max(maxFd, clientFd);
        }

        timeval timeout = {0, 500}; // Set select timeout to 1 second
        int activity = select(maxFd + 1, &readFds, nullptr, NULL, &timeout);

        if (activity < 0) {
            perror("Select error");
            break;
        }

        if (FD_ISSET(serverFd, &readFds)) {
            acceptConnections();
        }

        for (auto it = clients.begin(); it != clients.end();) {
            int clientFd = *it;
            if (FD_ISSET(clientFd, &readFds)) {
                if (!readRequest(clientFd)) {
                    it = clients.erase(it); // Remove disconnected client
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }

        handleRedisStream();
    }
}

void Server::acceptConnections() {
    int clientFd;
    do {
        clientFd = accept(serverFd, NULL, NULL);
        
        if (clientFd < 0) {

            if (errno != EWOULDBLOCK) {
                std::cout << "\nClient accept failed" << std::endl;
                perror("Accept error");
                return;
            }
            break;
        }

        // Set the client socket to non-blocking
        fcntl(clientFd, F_SETFL, O_NONBLOCK);

        clients.insert(clientFd);
        std::cout << "Accepted connection from client: " << clientFd << std::endl;
    } while (clientFd != -1);
}

bool Server::readRequest(int clientFd) {
    char data[512];
    memset(data, 0, sizeof(data));

    ssize_t bytesRead = recv(clientFd, data, sizeof(data), 0);
    if (bytesRead < 0) {
        perror("Read error");
        return false;
    } else if (bytesRead == 0) {
        std::cout << "Client " << clientFd << " disconnected" << std::endl;
        close(clientFd);
        return false;
    }

    std::string request(data, bytesRead);
    std::cout << "Received request from client " << clientFd << ": " << request << std::endl;

    std::string endpoint = parseEndpoint(request);
    std::cout << "Requested endpoint: " << endpoint << std::endl;
    
    ssize_t bytesSent = send(clientFd, data, bytesRead, 0);
    if (bytesSent < 0) {
        perror("Send error");
        return false;
    }

    return true;
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
    while (true) {
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter orchestrator BLOCK 100 COUNT 1 STREAMS %d-in >", idServer);

        assertReply(c2r, reply);

        if (ReadNumStreams(reply) == 0) {
            break;
        }
        
        char clientId[INT64_WIDTH];
        memset(clientId, 0, INT64_WIDTH);

        ReadStreamMsgVal(reply, 0, 0, 1, clientId);
        freeReplyObject(reply);

        sendToClient(std::atoi(clientId), "ok");
    }
}

void Server::sendToClient(int clientId, const std::string& message) {
    try {
        if (clients.find(clientId) != clients.end()) {
            send(clientId, message.c_str(), message.length(), 0);
        }
    } catch (std::exception& e) {
        std::cerr << "Send error for client " << clientId << ": " << e.what() << std::endl;
    }
}

Server::~Server() {
    close(serverFd);
    for (int clientFd : clients) {
        close(clientFd);
    }
}
