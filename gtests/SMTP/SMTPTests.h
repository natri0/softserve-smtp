#include <gtest/gtest.h>

#include "SMTPSession.h"
#include "SMTPContext.h"

using namespace ISXSMTP;

class SMTPTest : public testing::Test
{
protected:
	SMTPTest()
		: session(nullptr)
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
	ASSERT_STREQ("220 <domain> Service ready\r\n", session.OnConnect().c_str());
}

TEST_F(SMTPTest, BasicUsageTest)
{
	ASSERT_STREQ("220 <domain> Service ready\r\n", 
			session.OnConnect().c_str());

	ASSERT_STREQ("250-<domain> greets some@domain.com\r\n250 HELP\r\n",
			session.OnMessage("EHLO some@domain.com\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
			session.OnMessage("MAIL FROM:<reverse@path.com>\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
			session.OnMessage("RCPT TO:<forward1@path.com>\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n",
		session.OnMessage("RCPT TO:<forward2@path.com>\r\n").c_str());

	ASSERT_STREQ("354 Start mail input; end with <CRLF>.<CRLF>\r\n",
		session.OnMessage("DATA\r\n").c_str());

	ASSERT_STREQ("", session.OnMessage("First part of data\r\n").c_str());

	ASSERT_STREQ("250 Action completed\r\n", 
			session.OnMessage("Last part of data\r\n.\r\n").c_str());

	ASSERT_STREQ("221 <domain> Service closing transmission channel\r\n",
			session.OnMessage("QUIT\r\n").c_str());

	auto context = session.GetContext();

	ASSERT_STREQ(context->forward_path.GetString().c_str(), "forward1@path.com;forward2@path.com;");
	ASSERT_STREQ(context->reverse_path.GetString().c_str(), "reverse@path.com");
	ASSERT_STREQ(context->mail_data.GetString().c_str(), "First part of data\r\nLast part of data\r\n.\r\n");
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