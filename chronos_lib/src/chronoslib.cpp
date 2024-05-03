#include "chronoslib.h"

int _pid = 0;
int _logLvl = 0;
long double _currentTimestamp = 0;

redisContext *_c2r;
redisReply *_reply;
Logger _logger(LOG_FILE);

int connect(char *redisIP, int redisPort, int logLvl) {
    char value[VALUE_LEN];

    if (_pid != 0) {
        std::cout << "pid is different than -1, problably you use two times connect" << std::endl;
        return 1;
    }

    _c2r = redisConnect(redisIP, redisPort);
    initStreams(_c2r, REQUEST_CONNECTION);
    initStreams(_c2r, IDS_CONNECTION);

    _logLvl = logLvl;

    if (_logLvl > 0) {
        logRedis(static_cast<const char*>(REQUEST_CONNECTION), CONN_REQ, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XADD %s * request connection", REQUEST_CONNECTION);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    if (_logLvl > 0) {
        logRedis(static_cast<const char*>(IDS_CONNECTION), WAIT_ID, NULL_PARAM);
    }

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
    if (_logLvl >= 0) {
        logRedis((std::to_string(_pid) + "-orchestrator").c_str(), BLOCKING_CALL, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request alertBlocking", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void synSleep(long double T) {
    char buffer[VALUE_LEN];
    memset(buffer, '\0', VALUE_LEN);
    std::snprintf(buffer, VALUE_LEN, "%Lf", T);

    if (_logLvl >= 0) {
        logRedis((std::to_string(_pid) + "-orchestrator").c_str(), SYN_SLEEP, T);
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request synSleep time %s", _pid, buffer);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    if (_logLvl >= 0) {
        logRedis(("orchestrator-" + std::to_string(_pid)).c_str(), WAIT_SYNC, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS orchestrator-%d >", _pid);
    assertReply(_c2r, _reply);

    memset(buffer, '\0', VALUE_LEN);
    ReadStreamMsgVal(_reply, 0, 0, 1, buffer);

    _currentTimestamp = strtold(buffer, NULL);
    
    return;
}

void mySleep(long double T) {
    char buffer[VALUE_LEN];
    memset(buffer, '\0', VALUE_LEN);
    std::snprintf(buffer, VALUE_LEN, "%Lf", T);

    if (_logLvl >= 0) {
        logRedis((std::to_string(_pid) + "-orchestrator").c_str(), MY_SLEEP, T);
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request mySleep time %s", _pid, buffer);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    if (_logLvl >= 0) {
        logRedis(("orchestrator-" + std::to_string(_pid)).c_str(), WAIT_SYNC, NULL_PARAM);
    }   

    _reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS orchestrator-%d >", _pid);
    assertReply(_c2r, _reply);

    memset(buffer, '\0', VALUE_LEN);
    ReadStreamMsgVal(_reply, 0, 0, 1, buffer);

    _currentTimestamp = strtold(buffer, NULL);

    return;
}

void unblock() {
    if (_logLvl >= 0) {
        logRedis((std::to_string(_pid) + "-orchestrator").c_str(), UNBLOCK, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request alertUnblock", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void disconnect() {
    if (_logLvl >= 0) {
        logRedis((std::to_string(_pid) + "-orchestrator").c_str(), DISCONNECT, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request disconnect", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void logRedis(const char *stream, int message ,long double value) {
    std::string valueStr = std::to_string(value);

    if (NULL_PARAM == value) {
        valueStr = "";
    }

    _logger.redisLog(stream, message, valueStr);
    return;
}

long double getSimulationTimestamp() {
    return _currentTimestamp;
}