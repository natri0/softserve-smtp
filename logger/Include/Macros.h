#pragma once
#include "LogLevel.h"
#include "LogData.h"

#define ERROR_COLOR "\033[41m"
#define WARNING_COLOR "\033[43m"
#define INFORMATION_COLOR "\033[42m"
#define DEFAULT_COLOR "\033[0m"

#define DEFAULT_AMOUNT 30
#define DEFAULT_PATH ""
//#define DEFAULT_CONFIG false
#define DEFAULT_FLUSH true
#define DEFAULT_SIZE 8192
#define DEFAULT_END false
#define DEFAULT_LOG_LEVEL TRACE_LOG_LEVEL


#define NO_LOG_LEVEL LogLevel::NONE
#define PROD_LOG_LEVEL LogLevel::PROD
#define DEBUG_LOG_LEVEL LogLevel::DEBUG
#define TRACE_LOG_LEVEL LogLevel::TRACE

#define INFO_TYPE "[INFO]"
#define ERROR_TYPE "[ERROR]"
#define WARNING_TYPE "[WARNING]"

#define FUNCTION_NAME __FUNCTION__

//#define LOG_GET_THIS()      __if_exists(this) { this } __if_not_exists(this) { 0 }

#ifdef _MSC_VER
#   define LOG_GET_FUNC()      __FUNCTION__
#else
#   define LOG_GET_FUNC()      __PRETTY_FUNCTION__
#endif


#define LOG_GET_FILE()      __FILE__


#define IF_LOG_(level)   if (Logger::getInstance().blockLog(level)) { ; } else

//

#define LOG(level,type)  IF_LOG_(level) (Logger::getInstance()) += \
    LogData("", type, LOG_GET_FUNC(), level, std::this_thread::get_id()).ref()

#define LOG_INFO(level)           LOG(level, INFO_TYPE )
#define LOG_WARNING(level)        LOG(level, WARNING_TYPE )
#define LOG_ERROR(level)          LOG(level, ERROR_TYPE )