#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include "LogLevel.h"

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
    //void* object_ptr

    /**
     * @brief Constructs a new LogData object.
     */

    LogData(const std::string& m, const std::string& type, const std::string& loc,
        const LogLevel& level, std::thread::id id) //void* ptr = nullptr)
        : msg(m), type(type), location(loc), level(level), thr_id(id) { //, object_ptr(ptr) {
    }

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

    LogData& ref()
    {
        return *this;
    }


};