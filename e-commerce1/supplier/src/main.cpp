#include "../../classes/src/client.h"

#define ONE_SECOND 1
#define ONE_MINUTE 60 * ONE_SECOND
#define ONE_HOUR 60 * ONE_MINUTE
#define ONE_DAY 24 * ONE_HOUR


int main() {
    std::vector<int> sleepTimes = {2 * ONE_DAY, 10 * ONE_MINUTE, ONE_HOUR, ONE_SECOND};
    std::vector<std::string> requestTypes = {"addProduct", "viewStatistics"};
    std::vector<std::vector<long double>> matrix = {
                                                    {0, 0.3, 0.7, 0}, 
                                                    {0, 0.2, 0.1, 0.7}, 
                                                    {0, 0.1, 0.2, 0.7}, 
                                                    {1, 0, 0, 0, 0}
                                                   };
    std::vector<std::string> servers = {"127.0.0.1:2929"};

    Client client(sleepTimes, requestTypes, matrix, servers);
    client.run();

    return 0;
}