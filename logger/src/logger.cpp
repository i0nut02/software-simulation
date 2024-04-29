#include "logger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

Logger::Logger(const std::string& filename) : filename(filename) {
    file.open(filename, std::ios::app);
}

void Logger::log(LogType type, const std::string& stream, const std::string& message, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex); // Lock the mutex for this scope

    // Write to the file
    file << getCurrentTimeWithMilliseconds() << "; ";

    // Print log type
    switch (type) {
        case LogType::INFO:
            file << "INFO; ";
            break;
        case LogType::ERROR:
            file << "ERROR; ";
            break;
        case LogType::DEBUG:
            file << "DEBUG; ";
            break;
        case LogType::WARNING:
            file << "WARNING; ";
            break;
    }

    file << stream << "; " << message << "; " << value << std::endl;
    file.flush(); // Flush to ensure immediate write
}

void Logger::redisLog(const std::string& stream, int message, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex); // Lock the mutex for this scope

    // Write to the file
    file << getCurrentTimeWithMilliseconds() << ";";

    file << stream << ";" << message << ";" << value << std::endl;
    file.flush(); // Flush to ensure immediate write
}


std::string Logger::getCurrentTimeWithMilliseconds() const {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto now_ms_since_epoch = now_ms.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(now_ms_since_epoch);
    auto current_time = std::chrono::system_clock::to_time_t(now);
    auto ms_part = value.count() % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << ms_part;
    return ss.str();
}
