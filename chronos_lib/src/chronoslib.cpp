#include "chronoslib.h"

int _pid = 0;
redisContext *_c2r;
redisReply *_reply;
Con2DB _db = Con2DB(POSTGRESQL_SERVER, POSTGRESQL_PORT, POSTGRESQL_USER, POSTGRESQL_PSW, POSTGRESQL_DBNAME);
PGresult *_query_res;

int connect(char *redisIP, int redisPort) {
    char value[VALUE_LEN];
    if (_pid != 0) {
        std::cout << "pid is different than -1, problably you use two times connect" << std::endl;
        return 1;
    }

    _c2r = redisConnect(redisIP, redisPort);
    initStreams(_c2r, REQUEST_CONNECTION);
    initStreams(_c2r, IDS_CONNECTION);

    std::cout << "1 " << REQUEST_CONNECTION << " " << IDS_CONNECTION << std::endl;

    if (logRedis(static_cast<const char*>(REQUEST_CONNECTION), NULL_PARAM) != 0) {
        std::cout << "Error loging redis connection request" << std::endl;
        return 1;
    }

    std::cout << "1" << std::endl;

    _reply = RedisCommand(_c2r, "XADD %s * request connection", REQUEST_CONNECTION);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    std::cout << "1" << std::endl;

    if (logRedis(static_cast<const char*>(IDS_CONNECTION), NULL_PARAM) != 0) {
        std::cout << "Error loging redis command to take process ID" << std::endl;
        return 1;
    }

    std::cout << "1" << std::endl;

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
    if (logRedis((std::to_string(_pid) + "-orchestrator").c_str(), NULL_PARAM) != 0) {
        std::cout << "Error loging redis command to alert Blocking call" << std::endl;
        return;
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

    if (logRedis((std::to_string(_pid) + "-orchestrator").c_str(), T) != 0) {
        std::cout << "Error loging redis command to syn sleep request" << std::endl;
        return;
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request synSleep time %s", _pid, buffer);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    if (logRedis(("orchestrator-" + std::to_string(_pid)).c_str(), NULL_PARAM) != 0) {
        std::cout << "Error loging redis command to waiting for sync" << std::endl;
        return;
    }

    _reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS orchestrator-%d >", _pid);
    assertReply(_c2r, _reply);

    return;
}

void mySleep(long double T) {
    synSleep(T);
    return;
}

void disconnect() {
    if (logRedis((std::to_string(_pid) + "-orchestrator").c_str(), NULL_PARAM) != 0) {
        std::cout << "Error loging redis command to disconnect" << std::endl;
        return;
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request disconnect", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

int logRedis(const char *stream, long double value) {
    std::string query = std::string("INSERT INTO RedisLog VALUES(CURRENT_TIMESTAMP, \'") +
                        stream +
                        "\', " +
                        (value > 0 ? std::to_string(value) : std::string("NULL")) +
                        ")";

    std::string mutable_query = query;

    _query_res = _db.RunQuery(&mutable_query[0], false);
    if (PQresultStatus(_query_res) != PGRES_COMMAND_OK && PQresultStatus(_query_res) != PGRES_TUPLES_OK) {
        std::cout << "Error inserting into the Redis Log the listening action" << std::endl;
        return -1;
    }

    return 0;
}
