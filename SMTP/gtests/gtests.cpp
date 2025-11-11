#include <gtest/gtest.h>

#include "SMTPTests.h"
#include "SMTPCommandParserTests.h"
#include "SMTPCommandsTests.h"
#include "SMTPConfigBuilderTests.h"

#include "Profiler.h"

int main(int argc, char** argv) {
	PROFILE_BEGIN("gtest_profile");
	int res = 0;
	{
		PROFILE_SCOPE("gtest_main");
		testing::InitGoogleTest(&argc, argv);
	    res = RUN_ALL_TESTS();
	}
	PROFILE_END();
	return res;
}
