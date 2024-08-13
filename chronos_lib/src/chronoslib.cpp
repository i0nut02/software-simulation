#include "chronoslib.h"

int _pid = 0;
int _logLvl = 0;
long double _currentTimestamp = 0;
long double _efficienty = 0;

redisContext *_c2r;
redisReply *_reply;

void makeWaitUnlock() {
    _reply = RedisCommand(_c2r, "XADD %s * pid %d request waitUnlock", SEND_STREAM, _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    //_reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS orchestrator-%d >", _pid);
    //assertReply(_c2r, _reply);
}

int connect(char *redisIP, int redisPort, int logLvl) {
    char value[VALUE_LEN];
    
    if (_pid != 0) {
        std::cout << "pid is different than -1, problably you use two times connect" << std::endl;
        return 1;
    }

    _c2r = redisConnect(redisIP, redisPort);
    initStreams(_c2r, REQUEST_CONNECTION);
    initStreams(_c2r, IDS_CONNECTION);
    initStreams(_c2r, LOGGER_STREAM);

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

    std::string new_stream = "orchestrator-" + valStr;

    initStreams(_c2r, new_stream.c_str());
    initStreams(_c2r, SEND_STREAM);

    return 0;
}

void alertBlocking() {
    if (_logLvl >= 0) {
        logRedis(SEND_STREAM, BLOCKING_CALL, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XADD %s * pid %d request alertBlocking", SEND_STREAM, _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void synSleep(long double T) {
    _efficienty += T;
    char buffer[VALUE_LEN];
    memset(buffer, '\0', VALUE_LEN);
    std::snprintf(buffer, VALUE_LEN, "%Lf", T);

    if (_logLvl >= 0) {
        logRedis(SEND_STREAM, SYN_SLEEP, T);
    }

    _reply = RedisCommand(_c2r, "XADD %s * pid %d request synSleep time %s", SEND_STREAM, _pid, buffer);
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

    _reply = RedisCommand(_c2r, "XADD %s * pid %d request mySleep time %s", SEND_STREAM, _pid, buffer);
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
    char buffer[VALUE_LEN];
    memset(buffer, '\0', VALUE_LEN);
    
    if (_logLvl >= 0) {
        logRedis(SEND_STREAM, UNBLOCK, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XADD %s * pid %d request alertUnblock", SEND_STREAM, _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    _reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS orchestrator-%d >", _pid);
    assertReply(_c2r, _reply);

    memset(buffer, '\0', VALUE_LEN);
    ReadStreamMsgVal(_reply, 0, 0, 1, buffer);

    _currentTimestamp = strtold(buffer, NULL);

    return;
}

void sendId(std::string id) {
    _reply = RedisCommand(_c2r, "XADD %s * pid %d request registID id %s", SEND_STREAM, _pid, id.c_str());
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);
}

void sendTo(std::string id) {
    _reply = RedisCommand(_c2r, "XADD %s * pid %d request sendingTo id %s", SEND_STREAM, _pid, id.c_str());
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);
}

void disconnect() {
    if (_logLvl >= 0) {
        logRedis(SEND_STREAM, DISCONNECT, NULL_PARAM);
    }

    _reply = RedisCommand(_c2r, "XADD %s * pid %d request disconnect", SEND_STREAM, _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void logRedis(const char *stream, const char *message, long double value) {
    std::string valueStr = std::to_string(value);

    if (NULL_PARAM == value) {
        valueStr = "";
    }

    std::ostringstream oss;

    // Get the current time
    std::string timestamp = getCurrentTime();

    // Log the timestamp
    oss << timestamp << ";";

    // Log the stream, message, and value
    oss << stream << ";" << message << ";" << value;

    _reply = RedisCommand(_c2r, "XADD %s * string %s", LOGGER_STREAM, oss.str().c_str());
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm buf;
    localtime_r(&in_time_t, &buf);

    std::ostringstream oss;
    oss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
    return oss.str();
}

std::string getSimulationTimestamp() {
    return std::to_string(_currentTimestamp);
}