#pragma once

#include <gtest/gtest.h>

#include "SMTPConfigBuilder.h"

using namespace ISXSMTP;

class SMTPConfigBuilderTest : public testing::Test
{
protected:
	SMTPConfigBuilder config_builder;
};

TEST_F(SMTPConfigBuilderTest, BasicUsageTest)
{
	SMTPContext context;
	context.mail_data.Append("test_data");

	config_builder
		.SetDomain("domain.test")
		.SetContext(context);

	auto config = config_builder.Build();

	ASSERT_STREQ(config.domain.c_str(), "domain.test");
	ASSERT_STREQ(config.context.mail_data.GetString().c_str(), "test_data");
}

TEST_F(SMTPConfigBuilderTest, SetDefaultConfigTest)
{
	SMTPContext context;
	context.mail_data.Append("test_data");

	config_builder
		.SetDomain("domain.test")
		.SetContext(context);
	auto config = config_builder.Build();
	
	SMTPConfigBuilder::SetDefaultConfig(config);

	auto default_config = SMTPConfigBuilder::GetDefaultConfig();

	ASSERT_EQ(config.domain, default_config.domain);
	ASSERT_STREQ(config.context.mail_data.GetString().c_str(), default_config.context.mail_data.GetString().c_str());
}

TEST_F(SMTPConfigBuilderTest, SetCurrentAsDefaultConfigTest)
{
	SMTPContext context;
	context.mail_data.Append("test_data");

	config_builder
		.SetDomain("domain.test")
		.SetContext(context);
	auto config = config_builder.Build();

	config_builder.SetCurrentAsDefault();

	auto default_config = SMTPConfigBuilder::GetDefaultConfig();

	ASSERT_EQ(config.domain, default_config.domain);
	ASSERT_STREQ(config.context.mail_data.GetString().c_str(), default_config.context.mail_data.GetString().c_str());
}