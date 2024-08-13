#include "chronos.h"

Chronos::Chronos(int n, int logLvl) {
    this->numProcesses = n;
    this->upperRandInt = (n + 2) * (n + 2);
    this->c2r = redisConnect(REDIS_IP, REDIS_PORT);
    
    this->reply = RedisCommand(this->c2r, "DEL %s", CONNECTION_REQUEST_STREAM);
    assertReply(this->c2r, this->reply);

    this->reply = RedisCommand(this->c2r, "DEL %s", CONNECTION_ACCEPT_STREAM);
    assertReply(this->c2r, this->reply);

    this->reply = RedisCommand(this->c2r, "DEL %s", RECEIVE_STREAM);
    assertReply(this->c2r, this->reply);

    initStreams(this->c2r, CONNECTION_REQUEST_STREAM);
    initStreams(this->c2r, CONNECTION_ACCEPT_STREAM);
    initStreams(this->c2r, RECEIVE_STREAM);
    initStreams(this->c2r, LOGGER_STREAM);

    this->simulationTime = 0;
    this->disconnectedProcesses = 0;
    this->logLvl = logLvl;
}

int Chronos::addProcess() {
    int pid;
    srand((unsigned) time(NULL));

    do {
        pid = 1 + (rand() % this->upperRandInt);
    } while (this->processIDs.count(pid) != 0);

    this->processIDs.insert(pid);
    this->activeProcesses.insert(pid);

    std::string new_stream = "orchestrator-" + std::to_string(pid);

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

int Chronos::getNumDisconnections() {
    return this->disconnectedProcesses;
}

int Chronos::getNumBlockedProcesses(){
    return this->blockedProcesses.size();
}

int Chronos::getSizeActiveProcesses() {
    return this->activeProcesses.size();
}

int Chronos::acceptIncomingConn() {
    int pid;
    int count = 0;

    while (count < this->numProcesses) {
        if (this->logLvl > 0) {
            logRedis(CONNECTION_REQUEST_STREAM, READ_CONNECTIONS, NULL_VALUE);
        }
        
        this->reply = RedisCommand(this->c2r, "XREADGROUP GROUP diameter orchestrator BLOCK 0 COUNT 1 STREAMS %s >", CONNECTION_REQUEST_STREAM);
        assertReply(this->c2r, this->reply);

        if (ReadNumStreams(this->reply) == 0) {
            break;
        }

        pid = this->addProcess();

        if (this->logLvl > 0) {
            logRedis(CONNECTION_ACCEPT_STREAM, SEND_ID, pid);
        }

        this->reply = RedisCommand(this->c2r, "XADD %s * pid %d", CONNECTION_ACCEPT_STREAM, pid);
        assertReplyType(this->c2r, this->reply, REDIS_REPLY_STRING);
        freeReplyObject(this->reply);

        count++;
    }
    return 0;
}

int Chronos::handleEvents() {
    char key[KEY_LEN], value[VALUE_LEN], id[VALUE_LEN];
    int pid;

    std::set<int> copiedProcesses = this->activeProcesses;

    while (true) {
        //std::cout << "waitUnlockProcesses.size(): " << waitUnlockProcesses.size() << std::endl;
        //std::cout << "numProcesses: " << numProcesses << std::endl;
        //std::cout << "disconnectedProcesses: " << disconnectedProcesses << std::endl;
        //std::cout << "getNumBlockedProcesses(): " << getNumBlockedProcesses() << std::endl;
        //std::cout << "syncProcessesTime.size(): " << syncProcessesTime.size() << std::endl << std::endl;
        if ((waitUnlockProcesses.size() > 0) | ((this->numProcesses != this->disconnectedProcesses) && (static_cast<std::size_t>(this->numProcesses - this->disconnectedProcesses - this->getNumBlockedProcesses()) < this->syncProcessesTime.size()))) {
            this->reply = RedisCommand(this->c2r, "XREADGROUP GROUP diameter orchestrator BLOCK 0 COUNT 1 STREAMS %s >", RECEIVE_STREAM);
        } else {
            this->reply = RedisCommand(this->c2r, "XREADGROUP GROUP diameter orchestrator COUNT 1 STREAMS %s >", RECEIVE_STREAM);
        }
        assertReply(this->c2r, this->reply);

        if (ReadNumStreams(this->reply) == 0) {
            break;
        }

        memset(id, 0, VALUE_LEN);
        memset(key, 0, KEY_LEN);
        memset(value, 0, VALUE_LEN);

        ReadStreamMsgVal(this->reply, 0, 0, 1, id);
        ReadStreamMsgVal(this->reply, 0, 0, 2, key);
        ReadStreamMsgVal(this->reply, 0, 0, 3, value);

        if (strcmp(key, "request") != 0) {
            return 1;
        }

        pid = std::stoi(id);

        if (strcmp(value, "synSleep") == 0){
            this->handleSynSleepReq(pid);
        }

        if (strcmp(value, "alertBlocking") == 0){
            this->blockProcess(pid);
        }

        if (strcmp(value, "disconnect") == 0){
            this->handleDisconnection(pid);
        }

        if (strcmp(value, "mySleep") == 0){
            this->handleSynSleepReq(pid);
        }

        if (strcmp(value, "alertUnblock") == 0){
            this->unblockProcess(pid);
            if (waitUnlockProcesses.count(pid) != 0) {
                waitUnlockProcesses.erase(pid);
            }
            
            this->reply = RedisCommand(this->c2r, "XADD orchestrator-%d * request %s", pid, std::to_string(simulationTime).c_str());
            assertReplyType(this->c2r, this->reply, REDIS_REPLY_STRING);
            freeReplyObject(this->reply);
        }

        if (strcmp(value, "registID") == 0) {
            char id[VALUE_LEN];
            memset(id, 0, VALUE_LEN);

            ReadStreamMsgVal(reply, 0, 0, 5, id);
            ids[id] = pid;
        }

        if (strcmp(value, "sendingTo") == 0) {
            char id[VALUE_LEN];
            memset(id, 0, VALUE_LEN);

            ReadStreamMsgVal(reply, 0, 0, 5, id);
            // manage if blocking with map of integers or idk
            // if in blocking we are expecting that the first time will
            // be a unblock, but in the second one we expect a non blocking
            // and all the things.
            if (blockedProcesses.count(ids[id]) != 0) {
                waitUnlockProcesses.insert(ids[id]);
            }
        }
    }
    return 0;
}

void Chronos::handleSynSleepReq(int pid) {
    // In reply we have the T time to make pass
    char value[VALUE_LEN];

    if (!(ReadStreamMsgNumVal(this->reply, 0, 0) == 6)) {
        return;
    }

    this->requests += 1;

    if (this->blockedProcesses.count(pid) > 0) {
        this->unblockProcess(pid);
    }

    ReadStreamMsgVal(reply, 0, 0, 5, value);

    std::string strValue(value);

    std::pair<long double, int> pairToAdd;
    pairToAdd.first = std::stold(strValue) + this->simulationTime;
    pairToAdd.second = pid;

    this->syncProcessesTime.push(pairToAdd);
    this->activeProcesses.erase(pid);

    return;
}


void Chronos::handleDisconnection(int pid) {

    if (this->activeProcesses.count(pid) > 0) {
        // it can't be in synSleep handleTime; queue because it will wait
        this->activeProcesses.erase(pid);
        this->blockedProcesses.erase(pid);
    }
    this->disconnectedProcesses += 1;

    reply = RedisCommand(c2r, "DEL %d-orchestrator", pid);
    assertReply(c2r, reply);

    reply = RedisCommand(c2r, "DEL orchestrator-%d", pid);
    assertReply(c2r, reply);

    return;
}

int micro_sleep(long usec) {
    struct timespec ts;
    int res;

    if (usec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = usec / 1000000;
    ts.tv_nsec = (usec % 1000000) * 1000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void Chronos::handleTime() {
    if ((static_cast<std::size_t>(this->numProcesses - this->disconnectedProcesses - this->getNumBlockedProcesses()) == this->syncProcessesTime.size()) && !this->syncProcessesTime.empty()) {
        const auto& top = this->syncProcessesTime.top();
        this->simulationTime = top.first;
        
        char buffer[VALUE_LEN];
        memset(buffer, '\0', VALUE_LEN);
        std::snprintf(buffer, VALUE_LEN, "%Lf", this->simulationTime);
        
        while (!this->syncProcessesTime.empty() && this->syncProcessesTime.top().first == this->simulationTime) {
            if (this->logLvl >= 0) {
                logRedis(("orchestrator-" + std::to_string(this->syncProcessesTime.top().second)).c_str(), SYNC_PROCESS, this->syncProcessesTime.top().second);
            }

            this->reply = RedisCommand(this->c2r, "XADD orchestrator-%d * request %s", this->syncProcessesTime.top().second, buffer);
            assertReplyType(this->c2r, this->reply, REDIS_REPLY_STRING);
            freeReplyObject(this->reply);

            this->activeProcesses.insert(this->syncProcessesTime.top().second);
            this->syncProcessesTime.pop();
            break;
        }
    }
}

void Chronos::logRedis(const char *stream, const char *message, long double value) {
    std::string valueStr = std::to_string(value);

    if (NULL_VALUE == value) {
        valueStr = "";
    }

    std::ostringstream oss;

    // Get the current time
    std::string timestamp = getCurrentTime();

    // Log the timestamp
    oss << timestamp << ";";

    // Log the stream, message, and value
    oss << stream << ";" << message << ";" << value;

    this->reply = RedisCommand(this->c2r, "XADD %s * string %s", LOGGER_STREAM, oss.str().c_str());
    assertReplyType(this->c2r, this->reply, REDIS_REPLY_STRING);
    freeReplyObject(this->reply);

    return;
}

std::string Chronos::getCurrentTime() {
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