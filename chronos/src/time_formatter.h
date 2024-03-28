#ifndef TIME_FORMATTER_H
#define TIME_FORMATTER_H

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>

// Fully qualify the chrono namespace
typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

class TimeFormatter {
private:
    unsigned int years;
    unsigned int months;
    unsigned int days;

    unsigned int hours;
    unsigned int minutes;
    unsigned int seconds;
    unsigned int milliseconds;

public:
    TimeFormatter();
    TimeFormatter(std::string timeString);

    std::string toString() const;

    TimeFormatter& setYears(unsigned int _years);
    TimeFormatter& setMonths(unsigned int _months);
    TimeFormatter& setDays(unsigned int _days);
    TimeFormatter& setHours(unsigned int _hours);
    TimeFormatter& setMinutes(unsigned int _minutes);
    TimeFormatter& setSeconds(unsigned int _seconds);
    TimeFormatter& setMilliseconds(unsigned int _milliseconds);

    TimePoint addDuration(TimePoint time) const;
};

#endif
