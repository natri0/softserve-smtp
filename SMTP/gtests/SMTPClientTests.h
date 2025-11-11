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

TEST_F(SMTPClientTest, GenCommandsBasicTest)
{
	client.SetFrom("from@test.com");
	client.SetTo({ "to@test.com" });
	client.SetDomain("smtp.test");

	auto commands = client.GenCommands();

	ASSERT_STREQ(commands[0].c_str(), "EHLO smtp.test\r\n");
	ASSERT_STREQ(commands[1].c_str(), "MAIL FROM:<from@test.com>\r\n");
	ASSERT_STREQ(commands[2].c_str(), "RCPT TO:<to@test.com>\r\n");
	ASSERT_STREQ(commands[3].c_str(), "DATA\r\n");
	ASSERT_STREQ(commands[4].c_str(), "QUIT\r\n");
}

TEST_F(SMTPClientTest, OnReply2xxTest)
{
	auto res = client.OnReply("250 OK");
	ASSERT_EQ(res, SMTPTransactionStatus::SEND_NEXT_COMMAND);
}

TEST_F(SMTPClientTest, OnReply3xxTest)
{
	auto res = client.OnReply("354 Send data");
	ASSERT_EQ(res, SMTPTransactionStatus::SEND_DATA);
}

TEST_F(SMTPClientTest, OnReply4xxTest)
{
	auto res = client.OnReply("400 temp error");
	ASSERT_EQ(res, SMTPTransactionStatus::RETRY_LATER);
}

TEST_F(SMTPClientTest, OnReply5xxTest)
{
	auto res = client.OnReply("500 perm error");
	ASSERT_EQ(res, SMTPTransactionStatus::ABORTED);
}

TEST_F(SMTPClientTest, OnReplyMultiLineTest)
{
	auto res = client.OnReply("250-multiline");
	ASSERT_EQ(res, SMTPTransactionStatus::WAIT_FOR_REPLY);
}