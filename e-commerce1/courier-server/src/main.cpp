#include "../../classes/src/server.h"
#include <unordered_map>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " idServer" << std::endl;
        return 1;
    }

    try {
        int idServer = std::stoi(argv[1]);
        std::string redisIP = "127.0.0.1";
        int redisPort = 6379;

        // Define the service mapping
        std::unordered_map<std::string, std::string> serviceMap = {
            {"viewAvailableShippings", "viewAvailableShippings"},
            {"takeInChargeShipping", "takeInChargeShipping"},
            {"changeStatusDelivery", "changeStatusDelivery"}
        };

        Server server(redisIP, redisPort, idServer, serviceMap);
        server.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
