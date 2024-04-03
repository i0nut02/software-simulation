#include "main.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Please provide the number of processes as an argument." << std::endl;
        return 1;
    }
    
    int numProcesses = std::atoi(argv[1]);
    Chronos* ch = new Chronos(numProcesses);

    int count = 0;

    while (count < numProcesses) {
        count += ch->acceptIncomingConn();
    }

    if (count > numProcesses) {
        std::cout << "To many processes are connected: " << count << std::endl;
        return 1;
    }

    std::cout << "All the processes are connected" << std::endl;

    while (ch->getSizeActiveProcesses() != 0) {
        if (ch->handleEvents() != 0) {
            std::cout << "An error accour during event handling" << std::endl;
            return 1;
        }
        ch->handleTime();
    }

    delete ch;
    return 0;
}