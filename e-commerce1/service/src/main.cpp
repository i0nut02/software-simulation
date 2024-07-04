#include "../../classes/src/service.h"

#define ONE_SECOND 1
#define ONE_MINUTE 60 * ONE_SECOND
#define ONE_HOUR 60 * ONE_MINUTE
#define ONE_DAY 24 * ONE_HOUR


int main(int argc, char* argv[]) {
    // Verify the number of arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <idServer> <serviceName> <checkInterval>" << std::endl;
        return 1; // Return error code 1 if incorrect number of arguments
    }

    try {
        // Parse arguments
        int idServer = std::stoi(argv[1]);
        std::string serviceName = argv[2];
        long double checkInterval = std::stold(argv[3]);

        // Create Service instance
        Service service(idServer, serviceName, checkInterval);

        // Run the service
        service.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}