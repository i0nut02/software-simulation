#include "../../classes/src/service.h"

#define ONE_SECOND 1
#define ONE_MINUTE 60 * ONE_SECOND
#define ONE_HOUR 60 * ONE_MINUTE
#define ONE_DAY 24 * ONE_HOUR

#include <iostream>
#include <sstream>

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<long double> splitStringToLongDouble(const std::string& str, char delimiter) {
    std::vector<long double> values;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        values.push_back(std::stold(token));
    }
    return values;
}

int main(int argc, char* argv[]) {
    // Verify the number of arguments
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <idServer> <serviceName> <services> <times>" << std::endl;
        return 1; // Return error code 1 if incorrect number of arguments
    }

    try {
        // Parse arguments
        int idServer = std::stoi(argv[1]);
        std::string serviceName = argv[2];
        std::string servicesStr = argv[3];
        std::string timesStr = argv[4];

        // Split services and times
        std::vector<std::string> services = splitString(servicesStr, ',');
        std::vector<long double> times = splitStringToLongDouble(timesStr, ',');

        // Create Service instance
        Service service(idServer, serviceName, services, times);

        // Run the service
        service.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
