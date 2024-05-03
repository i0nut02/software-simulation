#include "main.h"


int c(int min, int max) {
    static bool initialized = false;
    if (!initialized) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        initialized = true;
    }
    return min + std::rand() % ((max + 1) - min);
}


long double getRandomReal(long double min, long double max) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<long double> dis(min, max);
    return dis(gen);
}


int main() {
    PGresult *query_res;
    char query[1000];

    if (connect() != 0) {
        return 1;
    }

    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    redisReply *reply;

    initStreams(c2r, "server");
    initStreams(c2r, "customer");

    long double T = 0;
    while (T < LAST) {
        long double g = getRandomReal(0.00001, 2 * ONEDAY);
        T += g;

        synSleep(g);

        reply = RedisCommand(c2r, "XADD server * request %d", _pid);
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