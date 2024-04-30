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

    switch (choice) {
        case 1:
            synSleep(T);
            break;
        case 2:
            alertBlocking();
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

    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    redisReply *reply;

    initStreams(c2r, "server");
    initStreams(c2r, "customer");

    long double T = 0;
    while (T < LAST) {
        long double g = getRandomNumber2(0.00001, 2 * ONEDAY);
        T += g;

        synSleep(g);

        reply = RedisCommand(c2r, "XADD server * request continue");
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);

        alertBlocking();

        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter boh BLOCK 0 COUNT 1 STREAMS customer >");
        assertReply(c2r, reply);

        unblock();
    }

    disconnect();
    return 0;
}