#include "pch.h"

#include "../../Include/Logger.h"
#include "../../Include/LogLevel.h"
#include "../../Include/Macros.h"

TEST(TestLogger, Singleton) {
	Logger& l1 = Logger::getInstance(DEBUG_LOG_LEVEL, "", 5);
	Logger& l2 = Logger::getInstance();

	ASSERT_EQ(&l1, &l2);
}

TEST(TestLogger, SetLevel) {
	Logger& logger = Logger::getInstance();
	logger.setLevel(TRACE_LOG_LEVEL);
	ASSERT_EQ(logger.getLevel(), TRACE_LOG_LEVEL);
}

TEST(TestLogger, BlockLog) {
	Logger& logger = Logger::getInstance();
	logger.setLevel(DEBUG_LOG_LEVEL);
	ASSERT_EQ(logger.blockLog(PROD_LOG_LEVEL), true);
	ASSERT_EQ(logger.blockLog(TRACE_LOG_LEVEL), false);
}

TEST(TestLogger, ToString) {
	Logger& logger = Logger::getInstance();
	ASSERT_EQ(logger.toString(LogLevel::TRACE), "TRACE");
	ASSERT_EQ(logger.toString(LogLevel::DEBUG), "DEBUG");
	ASSERT_EQ(logger.toString(LogLevel::PROD), "PROD");
	ASSERT_EQ(logger.toString(LogLevel::NONE), "NONE");

}
