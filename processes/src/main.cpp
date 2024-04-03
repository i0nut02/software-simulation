#include "main.h"

std::chrono::milliseconds getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

int getRandomNumber(int min, int max) {
    static bool initialized = false;
    if (!initialized) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        initialized = true;
    }
    return min + std::rand() % ((max + 1) - min);
}

long double getRandomNumber2(long double min, long double max) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<long double> dis(min, max);
    return dis(gen);
}

void executeRandomFunction() {
    int choice = getRandomNumber(1, 3);

    long double T = getRandomNumber2(5.0, 100.0);
    std::cout << T << std::endl;

    switch (choice) {
        case 1:
            synSleep(T);
            break;
        case 2:
            break;
        case 3:
            mySleep(T);
            break;
        default:
            std::cout << "Invalid choice\n";
    }
}

int main() {
    if (connect() != 0) {
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        executeRandomFunction();
    }

    disconnect();
    return 0;
}