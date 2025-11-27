#include "LogData.h"

LogData::LogData(std::string t, std::string m, std::string type, std::string loc,
        LogLevel level, std::thread::id id, std::string ft)
        : timestamp(std::move(t)), msg(std::move(m)), type(std::move(type)), location(std::move(loc)), level(level), thr_id(id), format(std::move(ft)) {
}



LogData& LogData::ref()
{
    return *this;
}


