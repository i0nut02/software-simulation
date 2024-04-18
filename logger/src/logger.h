#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    enum class LogType { INFO, ERROR, DEBUG, WARNING };

    Logger(const std::string& filename);

    void log(LogType type, const std::string& stream, const std::string& message, const std::string& value);
    void redisLog(const std::string& stream, const std::string& message, const std::string& value);

private:
    std::string filename;
    std::ofstream file;
    std::mutex mutex; // Mutex for thread-safe file access

    std::string getCurrentTimeWithMilliseconds() const;
};

#endif // LOGGER_H
