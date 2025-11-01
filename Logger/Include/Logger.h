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
#include "Macros.h"



const std::unordered_map<std::string, std::string> colored{
    {"[ERROR]", ERROR_COLOR},
    {"[WARNING]", WARNING_COLOR},
    {"[INFO]", INFORMATION_COLOR},
    {"[DEFAULT]", DEFAULT_COLOR}
};


class Logger {
private:
    mutable std::mutex mutex;
    std::ofstream file;
    boost::lockfree::queue<LogData*> queue;
    std::string location;
    std::thread thrd;
    std::string output_path;
    std::atomic<unsigned int> amount;
    std::atomic<bool> end;
    std::atomic<bool> do_flush;

    LogLevel local_level;

    Logger(const LogLevel&, const std::string&, const unsigned int);

    void fileInit(const unsigned int);

    void log(const std::string&, const std::string&, const std::string&, const LogLevel&, std::thread::id);//, void*);

    void log(const LogData&);


public:

    static Logger& getInstance(const LogLevel& level = DEFAULT_LOG_LEVEL, const std::string& path = DEFAULT_PATH, const unsigned int amount = DEFAULT_AMOUNT);

    void operator+=(const LogData& data);

    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger() = delete;

    ~Logger();

    bool blockLog(LogLevel level);

    void setOutputPath(const std::string& path);

    const std::string& getOutputPath() const;

    void setLevel(LogLevel level);

    void setFlush(const bool);

    const LogLevel& getLevel() const;

    std::string toString(LogLevel level);

    void flushMessage(const LogData& data, bool if_flush);

    void shutDown();




    void logError(const std::string&);

    void logWarning(const std::string&);

    void logInfo(const std::string&);

    void logFuncStart();

    void logFuncEnd();


    /*void logArguments()
    {
        m_real->save_to_queue({ "arguments: " + m_buff.get() }, "[INFO]", location, local_level);
        m_buff.clear();
    }

    template<typename T, typename... Args>
    void logArguments(const T& first, Args&... args)
    {
        if (static_cast<int>(local_level) == 3)
        {
            logArgument(first);
            logArguments(std::forward<Args>(args)...);
        }
        else
            this->logFuncStart();
    }

    template<typename T>
    void logReturn(const T& value)
    {
        if (static_cast<int>(local_level) == 3)
        {
            m_buff << value;
            temp_wrap::wrap_return(m_buff.get(), m_location, m_local_level, m_local_format);
            m_buff.clear();
        }
        else
            this->log_return_nothing();
    }*/


};
