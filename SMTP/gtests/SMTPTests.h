#pragma once

#include <gtest/gtest.h>

#include "SMTPDomain.h"
#include "SMTPContext.h"
#include "SMTPSession.h"
#include "SMTPConfigBuilder.h"
#include "SMTPConfig.h"

using namespace ISXSMTP;

class SMTPTest : public testing::Test
{
protected:
	SMTPTest()
		: session()
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

	SMTPSession session;
};

TEST_F(SMTPTest, OnConnectTest)
{
	ASSERT_STREQ(std::string("220 " + ISXSMTP::g_ServerDomain + " Service ready\r\n").c_str(), session.OnConnect().c_str());
}

TEST_F(SMTPTest, BasicUsageTest)
{
	ASSERT_STREQ(std::string("220 " + ISXSMTP::g_ServerDomain + " Service ready\r\n").c_str(),
		session.OnConnect().c_str());

	ASSERT_STREQ(std::string("250-" + ISXSMTP::g_ServerDomain + " greets some@domain.com\r\n250 HELP\r\n").c_str(),
		session.OnMessage("EHLO some@domain.com\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("MAIL FROM:<reverse@smtp.test>\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("RCPT TO:<forward1@smtp.test>\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("RCPT TO:<forward2@smtp.test>\r\n").c_str());

	ASSERT_STREQ("354 Start mail input; end with <CRLF>.<CRLF>\r\n",
		session.OnMessage("DATA\r\n").c_str());

	ASSERT_STREQ("", session.OnMessage("First part of data\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("Last part of data\r\n.\r\n").c_str());

	ASSERT_STREQ(std::string("221 " + ISXSMTP::g_ServerDomain + " Service closing transmission channel\r\n").c_str(),
		session.OnMessage("QUIT\r\n").c_str());

	auto context = session.GetContext();

	ASSERT_STREQ(context.forward_path.GetString().c_str(), "forward1@smtp.test;forward2@smtp.test;");
	ASSERT_STREQ(context.reverse_path.GetString().c_str(), "reverse@smtp.test");
	ASSERT_STREQ(context.mail_data.GetString().c_str(), "First part of data\r\nLast part of data\r\n.\r\n");
}

TEST_F(SMTPTest, CaseInsensitivityTest)
{
	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("rset\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("RSET\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("rSeT\r\n").c_str());
}