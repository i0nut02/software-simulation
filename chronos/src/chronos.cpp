#include "chronos.h"

Chronos::Chronos(int n) {
    this->numProcesses = n;
    this->upperRandInt = (n + 2) * (n + 2);
    this->c2r = redisConnect(REDIS_IP, REDIS_PORT);
    
    this->reply = RedisCommand(this->c2r, "DEL %s", CONNECTION_REQUEST_STREAM);
    assertReply(this->c2r, this->reply);

    this->reply = RedisCommand(this->c2r, "DEL %s", CONNECTION_ACCEPT_STREAM);
    assertReply(this->c2r, this->reply);

    initStreams(this->c2r, CONNECTION_REQUEST_STREAM);
    initStreams(this->c2r, CONNECTION_ACCEPT_STREAM);
}

int Chronos::addProcess() {
    int pid;
    srand((unsigned) time(NULL));

    do {
        pid = 1 + (rand() % this->upperRandInt);
    } while (this->processIDs.count(pid) != 0);

    this->processIDs.insert(pid);
    this->activeProcesses.insert(pid);

    std::string new_stream = std::to_string(pid) + "-orchestrator";
    
    this->reply = RedisCommand(this->c2r, "DEL %s", new_stream.c_str());
    assertReply(this->c2r, this->reply);
    initStreams(this->c2r, new_stream.c_str());

    new_stream = "orchestrator-" + std::to_string(pid);

    this->reply = RedisCommand(this->c2r, "DEL %s", new_stream.c_str());
    assertReply(this->c2r, this->reply);
    initStreams(c2r, new_stream.c_str());
    
    return pid;
}

int Chronos::blockProcess(int pid) {
    if (this->processIDs.count(pid) == 0) {
        return -1;
    }

    this->blockedProcesses.insert(pid);
    return 0;
}

void Chronos::unblockProcess(int pid) {
    this->blockedProcesses.erase(pid);
    return;
}

int Chronos::getNumProcesses() {
    return this->numProcesses;
}

int Chronos::getNumBlockedProcesses(){
    return this->blockedProcesses.size();
}

int Chronos::getSizeActiveProcesses() {
    return this->activeProcesses.size();
}

int Chronos::acceptIncomingConn() {
    int pid;
    int res = 0;

    while (1) {
        this->reply = RedisCommand(this->c2r, "XREADGROUP GROUP diameter orchestrator BLOCK %d COUNT 1 STREAMS %s >", MIN_BLOCK, CONNECTION_REQUEST_STREAM);
        assertReply(this->c2r, this->reply);

        if (ReadNumStreams(this->reply) == 0) {
            break;
        }

        pid = this->addProcess();

        this->reply = RedisCommand(this->c2r, "XADD %s * pid %d", CONNECTION_ACCEPT_STREAM, pid);
        assertReplyType(this->c2r, this->reply, REDIS_REPLY_STRING);
        freeReplyObject(this->reply);

        res++;
    }
    return res;
}

int Chronos::handleEvents() {
    char key[KEY_LEN], value[VALUE_LEN];

    for (auto pid : this->activeProcesses) {
        this->reply = RedisCommand(this->c2r, "XREADGROUP GROUP diameter orchestrator BLOCK %d COUNT 1 STREAMS %d-orchestrator >", MIN_BLOCK, pid);
        assertReply(this->c2r, this->reply);

        if (ReadNumStreams(this->reply) == 0) {
            continue;
        }

        memset(key, 0, KEY_LEN);
        memset(value, 0, VALUE_LEN);

        ReadStreamMsgVal(this->reply, 0, 0, 0, key);
        ReadStreamMsgVal(this->reply, 0, 0, 1, value);

        if (strcmp(key, "request") != 0) {
            return 1;
        }

        if (strcmp(value, "synSleep") == 0){
            this->handleSynSleepReq(pid);
        }

        if (strcmp(value, "alertBlocking") == 0){
            this->blockProcess(pid);
        }

        if (strcmp(value, "disconnect") == 0){
            this->handleDisconnection(pid);
        }
    }
    return 0;
}

void Chronos::handleSynSleepReq(int pid) {
    // In reply we have the T time to make pass
    char T[VALUE_LEN];

    if (!(ReadStreamMsgNumVal(this->reply, 0, 0) == 4)) {
        return;
    }

    if (this->blockedProcesses.count(pid) > 0) {
        this->unblockProcess(pid);
    }

    ReadStreamMsgVal(reply, 0, 0, 3, T);

    std::cout << T << std::endl;
}


void Chronos::handleDisconnection(int pid) {

    if (this->activeProcesses.count(pid) > 0) {
        // it can't be in synSleep queue because it will wait
        this->activeProcesses.erase(pid);
        this->blockedProcesses.erase(pid);
    }
    return;
}