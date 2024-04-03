#include "chronoslib.h"

int pid = 0;
redisContext *c2r;
redisReply *reply;

int connect(char *redisIP, int redisPort) {
    char value[VALUE_LEN];

    if (pid != 0) {
        std::cout << "pid is different that -1, problably you use two times connect" << std::endl;
        return 1;
    }

    c2r = redisConnect(redisIP, redisPort);
    initStreams(c2r, "request-connection");
    initStreams(c2r, "ids-connection");

    reply = RedisCommand(c2r, "XADD request-connection * request connection");
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);
    
    reply = RedisCommand(c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS ids-connection >");
    assertReply(c2r, reply);

    ReadStreamMsgVal(reply, 0, 0, 1, value);

    std::string valStr(value);

    pid = std::stod(value);

    std::string new_stream = valStr + "-orchestrator";

    initStreams(c2r, new_stream.c_str());

    new_stream = "orchestrator-" + valStr;

    initStreams(c2r, new_stream.c_str());

    return 0;
}

void alertBlocking() {
    reply = RedisCommand(c2r, "XADD %d-orchestrator * request alertBlocking", pid);
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

    return;
}

void synSleep(long double T) {
    std::cout << T << std::endl;
    std::cout << "SynSleep" << std::endl;

    char buffer[VALUE_LEN];
    memset(buffer, '\0', VALUE_LEN);
    std::snprintf(buffer, VALUE_LEN, "%Lf", T);

    std::cout << buffer << std::endl;

    reply = RedisCommand(c2r, "XADD %d-orchestrator * request synSleep time %s", pid, buffer);
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

    reply = RedisCommand(c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS orchestrator-%d >", pid);
    assertReply(c2r, reply);

    return;
}

void mySleep(long double T) {
    synSleep(T);
    return;
}

void disconnect() {
    reply = RedisCommand(c2r, "XADD %d-orchestrator * request disconnect", pid);
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

    return;
}