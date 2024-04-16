#include "main.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Please provide the number of processes as an argument." << std::endl;
        return 1;
    }
    
    int numProcesses = std::atoi(argv[1]);
    Chronos* ch = new Chronos(numProcesses);

    if (ch->acceptIncomingConn() != 0) {
        std::cout << "Error accepting connections" << std::endl;
        return 1;
    }

    std::cout << "All the processes are connected" << std::endl;

    auto now = std::chrono::system_clock::now();

    // Convert the time point to a time_t object
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t object to a string representation
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));

    // Print the string representation

    while (ch->getNumDisconnections() != ch->getNumProcesses()) {
        if (ch->handleEvents() != 0) {
            std::cout << "An error accour during event handling" << std::endl;
            return 1;
        }
        ch->handleTime();
    }
    std::cout << "Timestamp when all were connected: " << buffer << std::endl;

    now = std::chrono::system_clock::now();
    now_c = std::chrono::system_clock::to_time_t(now);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    std::cout << "Timestamp when all were disconnected: " << buffer << std::endl;
    
    delete ch;
    std::cout << "finish" << std::endl;
    return 0;
}