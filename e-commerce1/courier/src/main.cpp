#include "../../classes/src/client.h"
#include <iostream>

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

    // Example data to match the Client constructor signature
    std::vector<long double> sleepTimes = {2 * ONE_DAY, 10 * ONE_MINUTE, ONE_HOUR, ONE_HOUR, ONE_HOUR};
    std::vector<std::string> requestTypes = {"viewAvailableShippings", "takeInChargeShipping", "changeStatusDelivery"};
    std::vector<std::vector<long double>> matrix = {
        {0, 0.5, 0, 0.5, 0},
        {0, 0, 0.7, 0, 0.3},
        {0, 0, 0.3, 0.3, 0.4},
        {0, 0.2, 0, 0.3, 0.5},
        {1, 0, 0, 0, 0}
    };
    std::vector<int> servers;

    for (int i = 0; i < numServers; ++i) {
        servers.push_back(2000 + i);
    }

    std::string address = "localhost";
    int port = 6379;

    Client client(sleepTimes, requestTypes, matrix, servers, address, port);
    client.run();

    return 0;
}
