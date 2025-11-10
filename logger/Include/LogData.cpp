#include "LogData.h"

LogData::LogData(const std::string& m, const std::string& type, const std::string& loc,
        const LogLevel& level, std::thread::id id, std::string ft) //void* ptr = nullptr)
        : msg(m), type(type), location(loc), level(level), thr_id(id), format(ft) { //, object_ptr(ptr) {
        }



LogData& LogData::ref()
{
    return *this;
}


