#pragma once

#include <gtest/gtest.h>

#include "SMTPClient.h"

using namespace ISXSMTP;

class SMTPClientTest : public testing::Test
{
protected:
	SMTPClientTest()
		: client()
	{
	}

	void SetUp() override {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	void TearDown() override {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	SMTPClient client;
};

TEST_F(SMTPClientTest, ParseReplyBasicTest)
{
	auto res = client.ParseReply("250 test");

	ASSERT_EQ(res->GetCode(), 250);
	ASSERT_EQ(res->IsMultiLine(), false);
	ASSERT_STREQ("test", res->GetComment().c_str());
}

TEST_F(SMTPClientTest, ParseReplyNoCommentTest)
{
	auto res = client.ParseReply("250");

	ASSERT_EQ(res->GetCode(), 250);
	ASSERT_EQ(res->IsMultiLine(), false);
	ASSERT_STREQ("", res->GetComment().c_str());
}

TEST_F(SMTPClientTest, ParseReplyMultiLineNoCommentTest)
{
	auto res = client.ParseReply("250-");

	ASSERT_EQ(res->GetCode(), 250);
	ASSERT_EQ(res->IsMultiLine(), true);
	ASSERT_STREQ("", res->GetComment().c_str());
}

TEST_F(SMTPClientTest, ParseReplyMultiLineWithCommentTest)
{
	auto res = client.ParseReply("250-test");

	ASSERT_EQ(res->GetCode(), 250);
	ASSERT_EQ(res->IsMultiLine(), true);
	ASSERT_STREQ("test", res->GetComment().c_str());
}

TEST_F(SMTPClientTest, ParseReplyErrorTest)
{
	auto res = client.ParseReply("fdklfjldka jlkfda");
	
	ASSERT_EQ(res.has_value(), false);
}

TEST_F(SMTPClientTest, ParseReplyWronFormatTest)
{
	auto res = client.ParseReply("250test");

	ASSERT_EQ(res.has_value(), false);
}

