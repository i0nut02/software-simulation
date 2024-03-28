#include "time_formatter.h"

// Constructor
TimeFormatter::TimeFormatter() {
    this->years = 0; 
    this->months = 0;
    this->days = 0;
    this->hours = 0;
    this->minutes = 0;
    this->seconds = 0;
    this->milliseconds = 0;
}


TimeFormatter::TimeFormatter(const std::string timeString) {
    // Create a string stream to parse the input string
    std::istringstream iss(timeString);

    // Temporary variables to store parsed values
    char delimiter;
    int _years, _months, _days, _hours, _minutes, _seconds, _milliseconds;

    // Parse the string
    iss >> _years >> delimiter >> _months >> delimiter >> _days >> _hours >> delimiter >> _minutes >> delimiter >> _seconds >> delimiter >> _milliseconds;

    // Set the values to the object
    this->years = _years;
    this->months = _months;
    this->days = _days;
    this->hours = _hours;
    this->minutes = _minutes;
    this->seconds = _seconds;
    this->milliseconds = _milliseconds;
}

// Method to convert time to string
std::string TimeFormatter::toString() const {
    std::ostringstream oss;

    oss << years << "-"; // Year
    oss << months << "-"; // Month
    oss << days << " "; // Day
    oss << hours << ":"; // Hour
    oss << minutes << ":"; // Minute
    oss << seconds << "."; // Second
    oss << milliseconds; // Millisecond

    return oss.str();
}

// Setter methods
TimeFormatter& TimeFormatter::setYears(unsigned int _years) {
    years = _years;
    return *this;
}

TimeFormatter& TimeFormatter::setMonths(unsigned int _months) {
    months = _months;
    return *this;
}

TimeFormatter& TimeFormatter::setDays(unsigned int _days) {
    days = _days;
    return *this;
}

TimeFormatter& TimeFormatter::setHours(unsigned int _hours) {
    hours = _hours;
    return *this;
}

TimeFormatter& TimeFormatter::setMinutes(unsigned int _minutes) {
    minutes = _minutes;
    return *this;
}

TimeFormatter& TimeFormatter::setSeconds(unsigned int _seconds) {
    seconds = _seconds;
    return *this;
}

TimeFormatter& TimeFormatter::setMilliseconds(unsigned int _milliseconds) {
    milliseconds = _milliseconds;
    return *this;
}

TimePoint TimeFormatter::addDuration(TimePoint time) const {
    // Convert time components to durations
    std::chrono::duration<int64_t> duration_years = std::chrono::hours(24 * 365 * years);
    std::chrono::duration<int64_t> duration_months = std::chrono::hours(24 * 30 * months);
    std::chrono::duration<int64_t> duration_days = std::chrono::hours(24 * days);
    std::chrono::hours duration_hours(hours);
    std::chrono::minutes duration_minutes(minutes);
    std::chrono::seconds duration_seconds(seconds);
    std::chrono::milliseconds duration_milliseconds(milliseconds);

    // Add durations to input TimePoint
    time += duration_years;
    time += duration_months;
    time += duration_days;
    time += duration_hours;
    time += duration_minutes;
    time += duration_seconds;
    time += duration_milliseconds;

    return time;
}
