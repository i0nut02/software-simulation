#include "main.h"


int main() {
    redisReply* reply;
    redisContext* c2r;
    char value[LOG_LEN];

    std::ofstream file("../../log.txt", std::ios_base::trunc);

    if (!file) {
        std::cerr << "Error opening log file." << std::endl;
        return 0;
    }

    file << "timestamp;stream;action;value" << std::endl;

    c2r = redisConnect(REDIS_IP, REDIS_PORT);

    initStreams(c2r, LOGGER_STREAM);

    while (1) {
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter orchestrator BLOCK 15000 COUNT 1 STREAMS %s >", LOGGER_STREAM);
        assertReply(c2r, reply);

        if (ReadNumStreams(reply) == 0) {
            std::cout << "No log registered" << std::endl;
            break;
        }

        memset(value, 0, LOG_LEN);

        ReadStreamMsgVal(reply, 0, 0, 1, value);

        file << value << std::endl;

        freeReplyObject(reply);
    }

    file.close();

    reply = RedisCommand(c2r, "DEL %s", LOGGER_STREAM);
    assertReply(c2r, reply);

    return 0;
}
