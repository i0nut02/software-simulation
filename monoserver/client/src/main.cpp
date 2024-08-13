#include "main.h"
int micro_sleep(long usec) {
    struct timespec ts;
    int res;

    if (usec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = usec / 1000000;
    ts.tv_nsec = (usec % 1000000) * 1000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

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
    std::string curTime;

    if (connect() != 0) {
        return 1;
    }

    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    redisReply *reply;

    initStreams(c2r, "server");
    initStreams(c2r, "conn");
    initStreams(c2r, std::to_string(_pid).c_str());

    long double T = 0;
    char serverId[100];
    memset(serverId, 0, 100);
    reply = RedisCommand(c2r, "XREADGROUP GROUP diameter boh BLOCK 10000 COUNT 1 STREAMS conn >");
    assertReply(c2r, reply);
    if (ReadNumStreams(reply) == 0) {
        std::cout << "greve" << std::endl;
    }

    ReadStreamMsgVal(reply, 0, 0, 1, serverId);
    sendId(std::to_string(_pid));

    while (T < LAST) {
        long double g = getRandomReal(0.00001, 2 * ONEDAY);
        T += g;

        synSleep(g);

        curTime = getSimulationTimestamp();
        //makeWaitUnlock();
        sendTo(serverId);
        reply = RedisCommand(c2r, "XADD server * request %d time %s", _pid, curTime.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        synSleep(0.01L);

        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter boh COUNT 1 STREAMS %d >", _pid);

        if (ReadNumStreams(reply) == 0) {
            alertBlocking();
            reply = RedisCommand(c2r, "XREADGROUP GROUP diameter boh BLOCK 30000 COUNT 1 STREAMS %d >", _pid);
            assertReply(c2r, reply);
            unblock();
        } 
	}
	
    reply = RedisCommand(c2r, "DEL %d", _pid);
    assertReply(c2r, reply);

    disconnect();
    return 0;
}
