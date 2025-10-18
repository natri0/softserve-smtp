#include <gtest/gtest.h>

#include "SMTPTests.h"
#include "SMTPCommandParserTests.h"

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}