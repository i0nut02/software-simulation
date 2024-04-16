#include "chronoslib.h"

int _pid = 0;
redisContext *_c2r;
redisReply *_reply;
Logger _logger(LOG_FILE);

int connect(char *redisIP, int redisPort) {
    char value[VALUE_LEN];
    if (_pid != 0) {
        std::cout << "pid is different than -1, problably you use two times connect" << std::endl;
        return 1;
    }

    _c2r = redisConnect(redisIP, redisPort);
    initStreams(_c2r, REQUEST_CONNECTION);
    initStreams(_c2r, IDS_CONNECTION);

    logRedis(static_cast<const char*>(REQUEST_CONNECTION), "request of connection to orchestrator", NULL_PARAM);

    _reply = RedisCommand(_c2r, "XADD %s * request connection", REQUEST_CONNECTION);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    logRedis(static_cast<const char*>(IDS_CONNECTION), "waiting for pid", NULL_PARAM);

    _reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS %s >", IDS_CONNECTION);
    assertReply(_c2r, _reply);

    ReadStreamMsgVal(_reply, 0, 0, 1, value);

    std::string valStr(value);

    _pid = std::stod(value);

    std::string new_stream = valStr + "-orchestrator";

    initStreams(_c2r, new_stream.c_str());

    new_stream = "orchestrator-" + valStr;

    initStreams(_c2r, new_stream.c_str());

    return 0;
}

void alertBlocking() {
    logRedis((std::to_string(_pid) + "-orchestrator").c_str(), "alert blocking call", NULL_PARAM);

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request alertBlocking", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void synSleep(long double T) {
    char buffer[VALUE_LEN];
    memset(buffer, '\0', VALUE_LEN);
    std::snprintf(buffer, VALUE_LEN, "%Lf", T);

    logRedis((std::to_string(_pid) + "-orchestrator").c_str(), "syn sleep request", T);

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request synSleep time %s", _pid, buffer);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    logRedis(("orchestrator-" + std::to_string(_pid)).c_str(), "waiting for syncronization", NULL_PARAM);

    _reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS orchestrator-%d >", _pid);
    assertReply(_c2r, _reply);

    return;
}

void mySleep(long double T) {
    synSleep(T);
    return;
}

void disconnect() {
    logRedis((std::to_string(_pid) + "-orchestrator").c_str(), "disconnection", NULL_PARAM);

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request disconnect", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void logRedis(const char *stream, const char *message ,long double value) {
    std::string valueStr = std::to_string(value);

    if (NULL_PARAM == value) {
        valueStr = "";
    }

    _logger.log(Logger::LogType::INFO, stream, message, valueStr);
    return;
}
