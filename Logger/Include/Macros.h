#pragma once
#include "LogLevel.h"

#define ERROR_COLOR "\033[41m"
#define WARNING_COLOR "\033[43m"
#define INFORMATION_COLOR "\033[42m"
#define DEFAULT_COLOR "\033[0m"

#define DEFAULT_AMOUNT 30
#define DEFAULT_PATH ""
//#define DEFAULT_CONFIG false
//#define DEFAULT_FLUSH true

#define NO_LOG_LEVEL LogLevel::NONE
#define PROD_LOG_LEVEL LogLevel::PROD
#define DEBUG_LOG_LEVEL LogLevel::DEBUG
#define TRACE_LOG_LEVEL LogLevel::TRACE

#define DEFAULT_LOG_LEVEL NO_LOG_LEVEL

#define FUNCTION_NAME __FUNCTION__