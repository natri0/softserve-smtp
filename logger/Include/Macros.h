#ifndef MACROS_H
#define MACROS_H

#include "LogLevel.h"
#include "LogData.h"

class Logger;

#define ERROR_COLOR "\033[41m"
#define WARNING_COLOR "\033[43m"
#define INFORMATION_COLOR "\033[42m"
#define DEFAULT_COLOR "\033[0m"

#define DEFAULT_AMOUNT 30
#define DEFAULT_PATH ""
#define DEFAULT_FLUSH true
#define DEFAULT_SIZE 8192
#define DEFAULT_END false
#define DEFAULT_LOG_LEVEL TRACE

#define FORMAT_NO     ""
#define FORMAT_PROD   "[{0:T}]{0:t}| {0:m}"
#define FORMAT_DEBUG  "[{0:T}]{0:t}[{0:l}][{0:L}]| {0:m}"
#define FORMAT_TRACE  "[{0:T}]{0:t}[{0:l}][{0:L}][{0:i}]| {0:m}"

#define DEFAULT_FORMAT FORMAT_TRACE

#define SET_LEVEL(level) Logger::getInstance().setLevel(level)
#define GET_LEVEL() Logger::getInstance().getLevel()


#define NO LogLevel::None
#define PROD LogLevel::Prod
#define DEBUG LogLevel::Debug
#define TRACE LogLevel::Trace

#define INFO_TYPE "[INFO]"
#define ERROR_TYPE "[ERROR]"
#define WARNING_TYPE "[WARNING]"

#ifdef _MSC_VER
#   define LOG_GET_FUNC()      __FUNCTION__
#else
#   define LOG_GET_FUNC()      __PRETTY_FUNCTION__
#endif


#define LOG_GET_FILE()      __FILE__


#define IF_LOG(level)   if (Logger::getInstance().blockLog(level)) { ; } else

#define LOG(level,type) IF_LOG(level) (Logger::getInstance()) += \
    LogData("", type, LOG_GET_FUNC(), level, std::this_thread::get_id(), Logger::getInstance().getFormat()).ref()

#define LOG_INFO(level)           LOG(level, INFO_TYPE )
#define LOG_WARNING(level)        LOG(level, WARNING_TYPE )
#define LOG_ERROR(level)          LOG(level, ERROR_TYPE )

#endif
