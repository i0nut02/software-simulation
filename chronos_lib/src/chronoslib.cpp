#include "chronoslib.h"

int _pid = 0;
redisContext *_c2r;
redisReply *_reply;
Con2DB _db = Con2DB(POSTGRESQL_SERVER, POSTGRESQL_PORT, POSTGRESQL_USER, POSTGRESQL_PSW, POSTGRESQL_DBNAME);
PGresult *_query_res;

int connect(char *redisIP, int redisPort) {
    char value[VALUE_LEN];

    if (_pid != 0) {
        std::cout << "pid is different that -1, problably you use two times connect" << std::endl;
        return 1;
    }

    _c2r = redisConnect(redisIP, redisPort);
    initStreams(_c2r, "request-connection");
    initStreams(_c2r, "ids-connection");

    if (logRedis(NULL_PARAM, ORCHERTRATOR_ID, "request of connection", NULL_PARAM) != 0) {
        std::cout << "Error loging redis connection request" << std::endl;
        return 1;
    }

    _reply = RedisCommand(_c2r, "XADD request-connection * request connection");
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);


    if (logRedis(NULL_PARAM, ORCHERTRATOR_ID, "waiting for process ID", NULL_PARAM) != 0) {
        std::cout << "Error loging redis command to take process ID" << std::endl;
        return 1;
    }

    _reply = RedisCommand(_c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS ids-connection >");
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
    if (logRedis(_pid, ORCHERTRATOR_ID, "alert Blocking call", NULL_PARAM) != 0) {
        std::cout << "Error loging redis command to alert Blocking call" << std::endl;
        return;
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request alertBlocking", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

void synSleep(long double T) {
    std::cout << T << std::endl;
    std::cout << "SynSleep" << std::endl;

    char buffer[VALUE_LEN];
    memset(buffer, '\0', VALUE_LEN);
    std::snprintf(buffer, VALUE_LEN, "%Lf", T);

    std::cout << buffer << std::endl;

    if (logRedis(_pid, ORCHERTRATOR_ID, "syn sleep request", T) != 0) {
        std::cout << "Error loging redis command to syn sleep request" << std::endl;
        return;
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request synSleep time %s", _pid, buffer);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    if (logRedis(_pid, ORCHERTRATOR_ID, "waiting for sync", NULL_PARAM) != 0) {
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
    if (logRedis(_pid, ORCHERTRATOR_ID, "disconnection request", NULL_PARAM) != 0) {
        std::cout << "Error loging redis command to disconnect" << std::endl;
        return;
    }

    _reply = RedisCommand(_c2r, "XADD %d-orchestrator * request disconnect", _pid);
    assertReplyType(_c2r, _reply, REDIS_REPLY_STRING);
    freeReplyObject(_reply);

    return;
}

int logRedis(int from, int to, const char *action, long double reqVal) {
    std::string query = "INSERT INTO RedisLog VALUES(CURRENT_TIMESTAMP, \'"
        + std::string(action)
        + "\', "
        + (from >= 0 ? std::to_string(from) : std::string("NULL"))
        + ", "
        + (to >= 0 ? std::to_string(to) : std::string("NULL"))
        + ", "
        + (reqVal > 0 ? std::to_string(reqVal) : std::string("NULL"))
        + ")";

    std::string mutable_query = query;

    _query_res = _db.RunQuery(&mutable_query[0], false);
    if (PQresultStatus(_query_res) != PGRES_COMMAND_OK && PQresultStatus(_query_res) != PGRES_TUPLES_OK) {
        std::cout << "Error inserting into the Redis Log the listening action" << std::endl;
        return -1;
    }

    return 0;
}