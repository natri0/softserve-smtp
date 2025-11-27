#ifndef LOGGER_H
#define LOGGER_H


#include "LogData.h"
#include "LogLevel.h"
#include "Macros.h"

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


/**
 * @brief Asynchronous logger.
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
    std::string format;

    /**
     * @brief Private constructor (Singleton pattern).
     */

    Logger(LogLevel, std::string, std::uint32_t, bool, std::string);

    void fileInit(std::uint32_t);

    void log(std::string, std::string, std::string, std::string, LogLevel, std::thread::id);

    void log(LogData);

    /**
     * @brief Determines if a message should be blocked based on its log level.
     * @param level Log level of the message
     * @return true if log should be blocked, false otherwise
     */



    void writeLogToFile(const LogData& data);

    void writeLogToConsole(const LogData& data);

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

    static Logger& getInstance(LogLevel level = DEFAULT_LOG_LEVEL, std::string_view path = DEFAULT_PATH, std::uint32_t amount = DEFAULT_AMOUNT, bool do_flush = DEFAULT_FLUSH, std::string_view format = DEFAULT_FORMAT);



    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger() = delete;

    /**
     * @brief Destructor safely shuts down background thread and closes file.
     */

    ~Logger();

    std::vector<std::string> readAllLogs() const;

    std::vector<std::string> readLogsByKeyword(const std::string& keyword) const;

    void operator+=(const LogData& data);

    void setFormat(std::string format);

    std::string getFormat() const;

    void setOutputPath(std::string path);

    const std::string& getOutputPath() const;

    void setLevel(LogLevel level);

    void setFlush(bool);


    std::string chooseFormat(LogLevel level);

    const LogLevel& getLevel() const;

    /**
     * @brief Converts a log level to string (e.g., TRACE ? "TRACE").
     */

    static std::string getLevelName(LogLevel level);

    bool blockLog(LogLevel level);

    /**
     * @brief Stops the background thread and finalizes logging.
     */

    void shutDown();



};

#endif
