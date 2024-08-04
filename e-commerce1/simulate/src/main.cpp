#include <iostream>
#include <cstdlib>    // For system()
#include <sstream>    // For std::ostringstream
#include <vector>     // For std::vector
#include <string>     // For std::string
#include <unordered_map>
#include <functional>
#include <numeric>
#include <unistd.h>

std::unordered_map<std::string, std::vector<std::string>> entityServices = {
    {"courier", {"viewAvailableShippings", "takeInChargeShipping", "changeStatusDelivery"}},
    {"customer", {"searchProduct", "addToCart", "createOrder", "viewOrder"}},
    {"supplier", {"addProduct", "viewStatistics"}}
};

std::unordered_map<std::string, std::vector<std::string>> requestsServices = {
    {"viewAvailableShippings", {"viewAvailableShippings"}}, 
    {"takeInChargeShipping", {"takeInChargeShipping"}}, 
    {"changeStatusDelivery", {"changeStatusDelivery"}}, 
    {"searchProduct", {"searchProduct"}}, 
    {"addToCart", {"addToCart"}}, 
    {"createOrder", {"createOrder"}}, 
    {"viewOrder", {"viewOrder"}},
    {"addProduct", {"addProduct"}}, 
    {"viewStatistics", {"viewStatistics"}}
};

std::unordered_map<std::string, int> serverPort = {
    {"courier", 2000},
    {"customer", 2100},
    {"supplier", 2200}
};

std::unordered_map<std::string, long double> serviceTime = {
    {"viewAvailableShippings", 0.6L}, 
    {"takeInChargeShipping", 0.4L}, 
    {"changeStatusDelivery", 0.4L}, 
    {"searchProduct", 0.7L},
    {"addToCart", 0.4L}, 
    {"createOrder", 0.5L}, 
    {"viewOrder", 0.4L}, 
    {"addProduct", 0.5L}, 
    {"viewStatistics", 0.8L}
};

std::string extractEntity(const std::string& path) {
    int startPos = 6; // Length of "../../"

    // Find the position of "-"
    std::size_t endPos = path.find("-", startPos);
    if (endPos == std::string::npos) {
        return ""; // Return an empty string if "-" is not found
    }

    // Extract the substring between startPos and endPos
    return path.substr(startPos, endPos - startPos);
}

// Function to run the make command in the specified directory
void runMake(const std::string& directory) {
    std::ostringstream command;
    command << "make -C " << directory << " clean; " << "make -C " << directory;

    int result = system(command.str().c_str());

    if (result != 0) {
        std::cerr << "Error: Failed to execute make in directory " << directory << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Function to run a binary multiple times using system()
void runBinary(const std::string& binaryPath, int count, int numServers) {
    for (int i = 0; i < count; ++i) {
        std::ostringstream command;
        command << binaryPath << " " << numServers << " &"; // Build the command to run the binary in the background

        int result = system(command.str().c_str()); // Execute the command

        if (result != 0) {
            std::cerr << "Error: Failed to execute " << binaryPath << std::endl;
        }
    }
}

// Function to run a binary multiple times using system()
void runBinaryServer(const std::string& binaryPath, int count) {
    std::string entity = extractEntity(binaryPath);

    for (int i = 0; i < count; ++i) {
        std::ostringstream command;
        command << binaryPath << " " << serverPort[entity] + i << " &"; // Build the command to run the binary in the background

        int result = system(command.str().c_str()); // Execute the command

        if (result != 0) {
            std::cerr << "Error: Failed to execute " << binaryPath << std::endl;
        }

        for (std::string service : entityServices[entity]) {
            std::string requests = "";
            std::string times = "";

            for (std::string request : requestsServices[service]) {
                requests += request + ",";
                times += std::to_string(serviceTime[request]) + ",";
            }

            requests.pop_back(); // remove the last comma
            times.pop_back();
            
            std::ostringstream command2;
            command2 << "../../service/bin/main" << " " << serverPort[entity] + i << " " << service << " " << requests << " " << times << " &";
            result = system(command2.str().c_str());
            if (result != 0) {
                std::cerr << "Error: Failed to execute " << binaryPath << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " numCustomers numCustomerServers numCouriers numCouriersServers numSuppliers numSuppliersServers" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse command line arguments
    int numCustomers = std::stoi(argv[1]);
    int numCustomerServers = std::stoi(argv[2]);
    int numCouriers = std::stoi(argv[3]);
    int numCouriersServers = std::stoi(argv[4]);
    int numSuppliers = std::stoi(argv[5]);
    int numSuppliersServers = std::stoi(argv[6]);

    // Define paths to the source directories and binaries
    std::vector<std::string> sourceDirs = {
        "../../customer/src",
        "../../customer-server/src",
        "../../courier/src",
        "../../courier-server/src",
        "../../supplier/src",
        "../../supplier-server/src"
    };

    std::vector<std::string> binaries = {
        "../../customer/bin/main",
        "../../customer-server/bin/main",
        "../../courier/bin/main",
        "../../courier-server/bin/main",
        "../../supplier/bin/main",
        "../../supplier-server/bin/main"
    };

    std::vector<int> counts = {
        numCustomers,
        numCustomerServers,
        numCouriers,
        numCouriersServers,
        numSuppliers,
        numSuppliersServers
    };

    int sum = std::accumulate(counts.begin(), counts.end(), 0);

    for (const auto& pair : entityServices) {
        sum += pair.second.size();
    }

    // Run make for each source directory
    for (const std::string& dir : sourceDirs) {
        runMake(dir);
    }
    runMake("../../service/src");
    runMake("../../../chronos/src");
    runMake("../../../chronos_lib/src");
    runMake("../../../logger/src");
    runMake("../../monitor/src");

    std::ostringstream command2;
    command2 << "../../../logger/bin/main " << " &";

    int result = system(command2.str().c_str()); // Execute the command

    if (result != 0) {
        std::cerr << "Error: Failed to execute ../../../chronos/bin/" << std::endl;
    }

    std::ostringstream command;
    command << "../../../chronos/bin/main " << sum << " &";

    result = system(command.str().c_str()); // Execute the command

    if (result != 0) {
        std::cerr << "Error: Failed to execute ../../../chronos/bin/" << std::endl;
    }

    std::ostringstream command3;
    command3 << "../../monitor/bin/main " << numCustomers << " " << numCustomerServers << " " << numCouriers << " " << numCouriersServers << " " << numSuppliers << " " << numSuppliersServers << " &";

    result = system(command3.str().c_str()); // Execute the command

    if (result != 0) {
        std::cerr << "Error: Failed to execute ../../../chronos/bin/" << std::endl;
    }

    // Run each binary the specified number of times
    for (size_t i = 1; i < binaries.size(); i+=2) {
        runBinaryServer(binaries[i], counts[i]);
    }

    sleep(1);

    for (size_t i = 0; i < binaries.size(); i+=2) {
        runBinary(binaries[i], counts[i], counts[i+1]);
    }

    std::cout << "All binaries started." << std::endl;

    return 0;
}
