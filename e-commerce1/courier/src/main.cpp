#include "../../classes/src/client.h"


int main() {
    std::vector<int> sleepTimes = {10000, 5, 6, 7, 10};
    std::vector<std::string> requestTypes = {"prova1", "prova2", "prova3"};
    std::vector<std::vector<long double>> matrix = {{0, 0.5, 0.3, 0.2, 0}, {0, 0.2, 0.3, 0.2, 0.3}, {0, 0.5, 0, 0.2, 0.3}, {0, 0.4, 0.3, 0, 0.3}, {1, 0, 0, 0, 0}};
    std::vector<std::string> servers = {"127.0.0.1:2929"};

    Client client(sleepTimes, requestTypes, matrix, servers);
    std::cout << "yoyoy"<< std::endl;
    client.run();

    return 0;
}