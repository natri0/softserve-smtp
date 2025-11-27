#include "LogData.h"

LogData::LogData(const std::string& t,const std::string& m, const std::string& type, const std::string& loc,
        const LogLevel& level, std::thread::id id, std::string ft)
        :msg(m), type(type), location(loc), level(level), thr_id(id), format(ft), timestamp(t) {
}



LogData& LogData::ref()
{
    return *this;
}


