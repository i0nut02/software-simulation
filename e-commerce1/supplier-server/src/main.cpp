#include "../../classes/src/server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " idServer" << std::endl;
        return 1;
    }

    try {
        int idServer = std::stoi(argv[1]);
        std::string redisIP = "127.0.0.1";  // Replace with your actual Redis IP address
        int redisPort = 6379;  // Replace with your actual Redis port

        // Define the service mapping
        std::unordered_map<std::string, std::string> serviceMap = {
            {"addProduct", "addProduct"},
            {"viewStatistics", "viewStatistics"}
        };

        // Create the server instance
        Server server(redisIP, redisPort, idServer, serviceMap);
        server.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}