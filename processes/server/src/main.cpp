#include "main.h"


int main() {
    PGresult *query_res;
    char query[1000];

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
    
    long double T = 0;
    char id[100];

    while (1) {

        alertBlocking();
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter server BLOCK 10000 COUNT 1 STREAMS server >");
        assertReply(c2r, reply);

        if (ReadNumStreams(reply) == 0) {
            break;
        }

        memset(id, 0, 100);
        ReadStreamMsgVal(reply, 0, 0, 1, id);

        unblock();

        reply = RedisCommand(c2r, "XADD customer * request continue");
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);

        synSleep(1);
    }

    disconnect();
    
    reply = RedisCommand(c2r, "DEL server");
    assertReply(c2r, reply);
}