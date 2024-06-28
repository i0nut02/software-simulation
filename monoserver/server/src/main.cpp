#include "main.h"


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

    initStreams(c2r, "server");
    initStreams(c2r, "customer");
    initStreams(c2r, "monitor-monoserver");

    long double T = 0;
    char id[100], timeReq[1000];

    while (1) {

        alertBlocking();
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter server BLOCK 10000 COUNT 1 STREAMS server >");
        assertReply(c2r, reply);
        synSleep(1);

        if (ReadNumStreams(reply) == 0) {
            break;
        }

        memset(id, 0, 100);
        memset(timeReq, 0, 1000);
        ReadStreamMsgVal(reply, 0, 0, 1, id);
        ReadStreamMsgVal(reply, 0, 0, 3, timeReq);

        reply = RedisCommand(c2r, "XADD customer * request continue");
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);

        std::string endTime = std::to_string(getSimulationTimestamp());
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