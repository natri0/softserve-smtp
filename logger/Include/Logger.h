#pragma once
#include <iostream>
#include <fstream>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include <atomic>
#include <chrono>
#include <ctime>
#include <memory>
#include <unordered_map>
#include <filesystem>
#include <boost/lockfree/queue.hpp>
#include <shared_mutex>
#include <fstream>
#include "Macros.h"

/**
 * @brief Color codes for console output.
 */

const std::unordered_map<std::string, std::string> colored{
    {"[ERROR]", ERROR_COLOR},
    {"[WARNING]", WARNING_COLOR},
    {"[INFO]", INFORMATION_COLOR},
    {"[DEFAULT]", DEFAULT_COLOR}
};

/**
 * @brief Asynchronous thread-safe logger.
 *
 * The Logger class supports:
 *  - Asynchronous log processing using a background thread
 *  - Lock-free queue
 *  - Log file rotation and colored console output
 *  - Configurable log levels and output paths
 */

class Logger {
private:
    mutable std::shared_mutex mutex;
    std::ofstream file;
    boost::lockfree::queue<LogData*> queue;
    std::thread thrd;
    std::string output_path;
    std::atomic<unsigned int> amount;
    std::atomic<bool> end;
    std::atomic<bool> do_flush;
    LogLevel local_level;

    /**
     * @brief Private constructor (Singleton pattern).
     */

    Logger(const LogLevel&, const std::string&, const unsigned int, const bool);

    void fileInit(const unsigned int);

    void log(const std::string&, const std::string&, const std::string&, const LogLevel&, std::thread::id);//, void*);

    void log(const LogData&);

    /**
     * @brief Determines if a message should be blocked based on its log level.
     * @param level Log level of the message
     * @return true if log should be blocked, false otherwise
     */

    bool blockLog(LogLevel level);

    void write_log_to_file(const LogData& data);

    void write_log_to_console(const LogData& data);

    /**
     * @brief Flushes a message to output (file and/or console).
     * @param data Log record
     */

    void flushMessage(const LogData& data);

public:

    /**
     * @brief Returns the singleton logger instance.
     *
     * @param level Initial log level
     * @param path Output directory (optional)
     * @param amount Maximum number of log files to keep
     * @param do_flush Whether to print to console
     * 
     * @return Reference to the Logger instance
     */

    static Logger& getInstance(const LogLevel& level = DEFAULT_LOG_LEVEL, const std::string& path = DEFAULT_PATH, const unsigned int amount = DEFAULT_AMOUNT, const bool do_flush = DEFAULT_FLUSH);

    

    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger() = delete;

    /**
     * @brief Destructor — safely shuts down background thread and closes file.
     */

    ~Logger();

    std::vector<std::string> readAllLogs() const;

    std::vector<std::string> readLogsByKeyword(const std::string& keyword) const;

    void operator+=(const LogData& data);

    void setOutputPath(const std::string& path);

    const std::string& getOutputPath() const;

    void setLevel(LogLevel level);

    void setFlush(const bool);

    const LogLevel& getLevel() const;

    /**
     * @brief Converts a log level to string (e.g., TRACE ? "TRACE").
     */

    std::string toString(LogLevel level);

    
    /**
     * @brief Stops the background thread and finalizes logging.
     */

    void shutDown();


    // Common message shortcuts

    void logFuncStart();

    void logFuncEnd();

  
};