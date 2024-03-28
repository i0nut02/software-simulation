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

void executeRandomFunction() {
    int choice = getRandomNumber(1, 3);
    TimeFormatter T;

    T.setSeconds(getRandomNumber(5, 100));

    switch (choice) {
        case 1:
            std::cout << T.toString() << std::endl;
            synSleep(T);
            std::cout << getCurrentTime().count() << std::endl;
            break;
        case 2:
            break;
        case 3:
            std::cout << T.toString() << std::endl;
            mySleep(T);
            std::cout << getCurrentTime().count() << std::endl;
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