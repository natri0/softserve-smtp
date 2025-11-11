#ifndef FORMATTER_H
#define FORMATTER_H

#include <format>
#include <chrono>
#include <string>
#include <unordered_map>
#include <sstream>
#include "Macros.h"

/**
 * @brief Color codes for console output.
 */
extern const std::unordered_map<std::string, std::string> colored;

struct ConsoleLog {
    const LogData& ref;
};

template<>
class std::formatter<LogData>
{
public:
    bool thr_id = false, time = false, type = false, level = false, location = false, text = false;

    constexpr auto parse(std::format_parse_context& context);
    auto format(const LogData& obj, auto& context) const;
};

template<>
class std::formatter<ConsoleLog> : public std::formatter<LogData>
{
public:
    auto format(const ConsoleLog& obj, auto& context) const;
};

#endif
