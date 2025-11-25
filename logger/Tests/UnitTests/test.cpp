#include "pch.h"

#include "Logger.h"
#include "LogLevel.h"
#include "Macros.h"

TEST(TestLogger, Singleton) {
    Logger& l1 = Logger::getInstance(DEBUG, "", 5);
    Logger& l2 = Logger::getInstance();

    ASSERT_EQ(&l1, &l2);
}

TEST(TestLogger, SetLevel) {
    Logger& logger = Logger::getInstance();
    logger.setLevel(TRACE);
    ASSERT_EQ(logger.getLevel(), TRACE);
}

TEST(TestLogger, BlockLog) {
    Logger& logger = Logger::getInstance();
    logger.setLevel(DEBUG);
    ASSERT_EQ(logger.blockLog(PROD), false);
    ASSERT_EQ(logger.blockLog(TRACE), true);
}

TEST(TestLogger, ToString) {
    Logger& logger = Logger::getInstance();
    ASSERT_EQ(Logger::getLevelName(LogLevel::Trace), "TRACE");
    ASSERT_EQ(Logger::getLevelName(LogLevel::Debug), "DEBUG");
    ASSERT_EQ(Logger::getLevelName(LogLevel::Prod), "PROD");
    ASSERT_EQ(Logger::getLevelName(LogLevel::None), "NONE");

}
