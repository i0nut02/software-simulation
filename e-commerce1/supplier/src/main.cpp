#include "../../classes/src/client.h"
#include <iostream>
#include <vector>
#include <string>

#define ONE_SECOND 1
#define ONE_MINUTE 60 * ONE_SECOND
#define ONE_HOUR 60 * ONE_MINUTE
#define ONE_DAY 24 * ONE_HOUR

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_servers>" << std::endl;
        return 1;
    }

    int numServers = std::stoi(argv[1]);

    std::vector<long double> sleepTimes = {2 * ONE_DAY, 10 * ONE_MINUTE, ONE_HOUR, ONE_SECOND};
    std::vector<std::string> requestTypes = {"addProduct", "viewStatistics"};
    std::vector<std::vector<long double>> matrix = {
        {0, 0.3, 0.7, 0}, 
        {0, 0.2, 0.1, 0.7}, 
        {0, 0.1, 0.2, 0.7}, 
        {1, 0, 0, 0, 0}
    };
    std::vector<int> servers;

    for (int i = 0; i < numServers; ++i) {
        servers.push_back(2200 + i);
    }

    std::string address = "localhost";
    int port = 6379;

    Client client(sleepTimes, requestTypes, matrix, servers, address, port);
    client.run();

    return 0;
}
