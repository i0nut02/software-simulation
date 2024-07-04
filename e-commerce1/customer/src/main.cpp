#include "../../classes/src/client.h"

#define ONE_SECOND 1
#define ONE_MINUTE 60 * ONE_SECOND
#define ONE_HOUR 60 * ONE_MINUTE
#define ONE_DAY 24 * ONE_HOUR


int main() {
    std::vector<int> sleepTimes = {5 * ONE_DAY, 20 * ONE_MINUTE, 16 * ONE_MINUTE, 20 * ONE_MINUTE, ONE_MINUTE};
    std::vector<std::string> requestTypes = {"searchProduct", "addToCart", "createOrder", "viewOrder"};
    std::vector<std::vector<long double>> matrix = {
                                                    {0, 1, 0, 0, 0, 0}, 
                                                    {0, 0.4, 0.2, 0, 0, 0.4}, 
                                                    {0, 0.8, 0.1, 0.1, 0, 0}, 
                                                    {0, 0, 0, 0, 0.7, 0.3},
                                                    {0, 0.1, 0, 0, 0, 0.9}, 
                                                    {1, 0, 0, 0, 0, 0}
                                                   };
    std::vector<std::string> servers = {"127.0.0.1:4949"};

    Client client(sleepTimes, requestTypes, matrix, servers);
    client.run();

    return 0;
}