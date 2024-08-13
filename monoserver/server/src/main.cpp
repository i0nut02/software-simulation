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

int main() {
    if (connect() != 0) {
        return 1;
    }

    redisContext *c2r = redisConnect(REDIS_IP, REDIS_PORT);
    redisReply *reply;

    reply = RedisCommand(c2r, "DEL server");
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL customer");
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL conn");
    assertReply(c2r, reply);

    initStreams(c2r, "server");
    initStreams(c2r, "customer");
    initStreams(c2r, "monitor-monoserver");
    initStreams(c2r, "conn");

    long double T = 0;
    char id[100], timeReq[1000];

    for (int i = 0; i < 2100; i++) {
        reply = RedisCommand(c2r, "XADD conn * request %d", _pid);
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
    }

    sendId(std::to_string(_pid));
    while (1) {
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter server COUNT 1 STREAMS server >");

        if (ReadNumStreams(reply) == 0) {
            alertBlocking();
            reply = RedisCommand(c2r, "XREADGROUP GROUP diameter server BLOCK 10000 COUNT 1 STREAMS server >");
            assertReply(c2r, reply);
            unblock();
        }
        synSleep(1);

        if (ReadNumStreams(reply) == 0) {
            std::cout << "ok" << std::endl;
            break;
        }

        memset(id, 0, 100);
        memset(timeReq, 0, 1000);
        ReadStreamMsgVal(reply, 0, 0, 1, id);
        ReadStreamMsgVal(reply, 0, 0, 3, timeReq);

        initStreams(c2r, id);
        sendTo(id);
        reply = RedisCommand(c2r, "XADD %s * request continue", id);
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);
        synSleep(0.01L);

        std::string endTime = getSimulationTimestamp();
        reply = RedisCommand(c2r, "XADD monitor-monoserver * startResponse %s endResponse %s", timeReq, endTime.c_str());
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);

    }
    disconnect();
    
    reply = RedisCommand(c2r, "DEL server");
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL customer");
    assertReply(c2r, reply);
}
