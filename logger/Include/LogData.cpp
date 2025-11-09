#include "LogData.h"

LogData::LogData(const std::string& m, const std::string& type, const std::string& loc,
        const LogLevel& level, std::thread::id id, std::string format) //void* ptr = nullptr)
        : msg(m), type(type), location(loc), level(level), thr_id(id), ft(format) { //, object_ptr(ptr) {
        //ft = Logger::chooseFormat(level);
        }



LogData& LogData::ref()
{
    return *this;
}