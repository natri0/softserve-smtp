#pragma once
#include <iostream>
#include <string>
#include <thread>
#include "LogLevel.h"
#include "Macros.h"
#include "Logger.h"

/**
 * @brief Represents a single log record.
 *
 * Contains log message, message type (e.g., [INFO]),
 * source location (usually function name), log level, and thread ID.
 */

struct LogData {
    std::string msg;
    std::string type;
    std::string location;
    LogLevel level;
    std::thread::id thr_id;
    std::string ft;
    //void* object_ptr

    /**
     * @brief Constructs a new LogData object.
     */

    LogData(const std::string& m, const std::string& type, const std::string& loc,
        const LogLevel& level, std::thread::id id, std::string format);

    /**
     * @brief Appends a value to the message text.
     *
     * Allows chaining using the << operator, similar to std::ostringstream.
     *
     * @param T Type of appended value
     * @param value Value to append
     * @return Reference to the modified LogData
     */

    template<typename T>
    LogData& operator<<(const T& value) {
        std::ostringstream oss;
        oss << value;
        msg += oss.str();
        return *this;
    }

    /**
     * @brief Returns a reference to the current object.
     *
     * Used for chaining in macros like LOG_INFO(level) << "message";
     */

    LogData& ref();


    std::format_args format_args() const;


};

const std::unordered_map<std::string, std::string> colored{
    {"[ERROR]", ERROR_COLOR},
    {"[WARNING]", WARNING_COLOR},
    {"[INFO]", INFORMATION_COLOR},
    {"[DEFAULT]", DEFAULT_COLOR}
};

struct ConsoleLog {
    const LogData& ref;

    auto format_args() const
    {
        return ref.format_args();
    }
};

template<>
class std::formatter<LogData>
{
public:
    bool thr_id = false, time = false, type = false, level = false, location = false, text = false;

    constexpr auto parse(std::format_parse_context& context)
    {
        auto it = context.begin();
        if (it == context.end()) return it;

        switch (*it)
        {
        case 'i': thr_id = true; break;
        case 'T': time = true; break;
        case 't': type = true; break;
        case 'l': level = true; break;
        case 'L': location = true; break;
        case 'm': text = true; break;
        default:
            throw std::format_error("Invalid placeholder");
        }

        ++it;
        if (it != context.end() && *it != '}')
            throw std::format_error("Invalid format args.");

        return it;
    }

    auto format(const LogData& obj, auto& context) const
    {
        std::string formatted;

        if (thr_id)  formatted += std::to_string(std::hash<std::thread::id>{}(obj.thr_id));
        if (time)    formatted += std::format("{:%H.%M.%S-%d.%m.%y}", std::chrono::system_clock::now());
        if (type)    formatted += obj.type;
        if (level)    formatted += Logger::toString(obj.level);
        if (location) formatted += obj.location;
        if (text)    formatted += obj.msg;

        return std::ranges::copy(std::move(formatted), context.out()).out;
    }
};


template<>
class std::formatter<ConsoleLog> : public std::formatter<LogData>
{
public:
    auto format(const ConsoleLog& obj, auto& context) const
    {
        std::string formatted;

        if (thr_id)  formatted += std::to_string(std::hash<std::thread::id>{}(obj.ref.thr_id));
        if (time) formatted += std::format("{:%H.%M.%S-%d.%m.%y}", std::chrono::system_clock::now());

        if (type) {
            auto it = colored.find(obj.ref.type);
            if (it != colored.end())
                formatted += it->second + obj.ref.type + DEFAULT_COLOR;
            else
                formatted += obj.ref.type;

        }
        if (level)   formatted += Logger::toString(obj.ref.level);
        if (location) formatted += obj.ref.location + " ";
        if (text)    formatted += obj.ref.msg;

        return std::ranges::copy(formatted, context.out()).out;
    }
};
