#include "main.h"


int main() {
    PGresult *query_res;
    Con2DB db(POSTGRESQL_SERVER, POSTGRESQL_PORT, POSTGRESQL_USER, POSTGRESQL_PSW, POSTGRESQL_DBNAME);
    char tmp_query[6000];
    std::string query = "INSERT INTO Logs VALUES ";
    int counter = 1;

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
    char id[100], timeReq[VALUE_LEN];

    while (1) {

        alertBlocking();
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter server BLOCK 10000 COUNT 1 STREAMS server >");
        assertReply(c2r, reply);
        synSleep(1);

        if (ReadNumStreams(reply) == 0) {
            break;
        }

        memset(id, 0, 100);
        memset(timeReq, 0, VALUE_LEN);
        ReadStreamMsgVal(reply, 0, 0, 1, id);
        ReadStreamMsgVal(reply, 0, 0, 3, timeReq);

        reply = RedisCommand(c2r, "XADD customer * request continue");
        assertReplyType(c2r, reply, REDIS_REPLY_STRING);
        freeReplyObject(reply);

        if (counter % 300 != 0) { 
            sprintf(tmp_query, "(%s, %s, %Lf),", id, timeReq, getSimulationTimestamp());
            query += tmp_query;
        } else {
            sprintf(tmp_query, "(%s, %s, %Lf);", id, timeReq, getSimulationTimestamp());
            query += tmp_query;
            query_res = db.ExecSQLcmd(&query[0]);
            counter = 1;
            query = "INSERT INTO Logs VALUES ";
        }
        counter += 1;
    }

    if (query.back() == ',') {
        query.back() = ';';
        query_res = db.ExecSQLcmd(&query[0]);
    }
    
    db.finish();
    disconnect();
    std::cout << "yoyoyoy" << std::endl;
    reply = RedisCommand(c2r, "DEL server");
    assertReply(c2r, reply);
}