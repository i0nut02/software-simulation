#include "../../classes/src/server.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./main idServer port" << std::endl;
        return 1;
    }

    try {
        int idServer = std::stoi(argv[1]);
        int port = std::stoi(argv[2]);
        std::cout <<  idServer << "  " << port << std::endl;
        std::string ipAddress = "127.0.0.1";  // Replace with your actual IP address
        std::vector<std::string> services = {"prova", "prova2", "prova3"};  // Replace with your actual message list

        Server server(ipAddress, port, services, 1, 2, 3, idServer);

        server.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

