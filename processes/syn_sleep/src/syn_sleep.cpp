#include "syn_sleep.h"

int synSleep(int t) {
    // assume we recconect to redis each time
    redisContext *c2r;
    redisReply *reply;

    int k, i, h;
    long val;
    char key[200], process_stream[100], value[200];
    int pid = getpid();

    sprintf(process_stream, "in-%d", pid);

    c2r = redisConnect(REDIS_IP, REDIS_PORT);
    initStreams(c2r, ORCHESTRATOR_STREAM);
    initStreams(c2r, process_stream);

    // send to orchestrator
    reply = RedisCommand(c2r, "XADD %s * time %ld", ORCHESTRATOR_STREAM, t);

    assertReplyType(c2r, reply, REDIS_REPLY_STRING);
    freeReplyObject(reply);

    // wait until t <= 0 
    do {
        // barrier syncronization (blocking read)
        reply = RedisCommand(c2r, "XREADGROUP GROUP diameter process BLOCK 0 COUNT 1 STREAMS %s >", PROCESS_STREAM);
        printf("exited the blocked\n");
        assertReply(c2r, reply);

        for (k = 0; k < ReadNumStreams(reply); k++) {
            printf("there is a stream\n");
            for (i = 0; i < ReadStreamNumMsg(reply, k); i++) {
                for (h = 0; h < ReadStreamMsgNumVal(reply, k, i); h += 2) {
                    ReadStreamMsgVal(reply, k, i, h, key);
                    ReadStreamMsgVal(reply, k, i, h +1, value);

                    if (!strcmp(key, "new_t")) {
                        char* check;

                        val = strtol(value, &check, 10);
                        if (*check) {
                            printf("error accoured\n");
                            return -1;
                        }
                        t -= val;
                        printf("%ld\n", val);

                    } else {
                        continue;
                    }
                }
            }
        }
        micro_sleep(10000);
    } while (t > 0);
    return 0;
}
