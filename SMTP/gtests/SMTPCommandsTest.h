#pragma once

#include <gtest/gtest.h>

#include "Commands/EHLOCommand.h"
#include "Commands/MAILCommand.h"
#include "Commands/RCPTCommand.h"
#include "Commands/DATACommand.h"
#include "Commands/NOOPCommand.h"
#include "Commands/HELOCommand.h"
#include "Commands/RSETCommand.h"
#include "Commands/QUITCommand.h"
#include "Commands/HELPCommand.h"
#include "Commands/VRFYCommand.h"
#include "SMTPReply.h"
#include "SMTPContext.h"
#include "SMTPSession.h"

#include <unordered_map>
#include <memory>

TEST(SMTPCommandsTest, EHLOTest)
{
	EHLOCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["domain"] = "test.string";

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("250-" + ISXSMTP::g_ServerDomain + " greets test.string\r\n250 HELP\r\n").c_str(),
		response.c_str());
	ASSERT_EQ(context.state, SMTPStates::POST_EHLO);

}

TEST(SMTPCommandsTest, EHLONoArgsTest)
{
	EHLOCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	//args.arguments["domain"] = "test.string";

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("501 Syntax error in parameters or arguments\r\n").c_str(),
		response.c_str());

}

TEST(SMTPCommandsTest, HELOTest)
{
	HELOCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["domain"] = "test.string";

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("250 " + ISXSMTP::g_ServerDomain + " greets test.string\r\n").c_str(),
		response.c_str());
	ASSERT_EQ(context.state, SMTPStates::POST_EHLO);
}

TEST(SMTPCommandsTest, HELONoArgsTest)
{
	HELOCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	//args.arguments["domain"] = "test.string";

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("501 Syntax error in parameters or arguments\r\n").c_str(),
		response.c_str());

}

TEST(SMTPCommandsTest, MAILTest)
{
	MAILCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["reverse_path"] = "reverse@path.com";
	context.state = SMTPStates::POST_EHLO;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("250 Action completed\r\n").c_str(),
		response.c_str());
	ASSERT_EQ(context.state, SMTPStates::POST_MAIL);
	ASSERT_STREQ(context.reverse_path.GetString().c_str(), "reverse@path.com");
}

TEST(SMTPCommandsTest, MAILBadSequenceTest)
{
	MAILCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["reverse_path"] = "reverse@path.com";
	context.state = SMTPStates::POST_RCPT;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("503 Bad sequence of commands\r\n").c_str(),
		response.c_str());
}

TEST(SMTPCommandsTest, MAILBadArgTest)
{
	MAILCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["reverse_path"] = "not mail address";
	context.state = SMTPStates::POST_EHLO;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("553 Requested action not taken: mailbox syntax is incorrect\r\n").c_str(),
		response.c_str());
}

TEST(SMTPCommandsTest, RCPTTest)
{
	RCPTCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["forward_path"] = "forward@smtp.test";
	context.state = SMTPStates::POST_MAIL;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("250 Action completed\r\n").c_str(),
		response.c_str());
	ASSERT_EQ(context.state, SMTPStates::POST_RCPT);
	ASSERT_STREQ(context.forward_path.GetString().c_str(), "forward@smtp.test;");
}

TEST(SMTPCommandsTest, RCPTBadSequenceTest)
{
	RCPTCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["forward_path"] = "forward@path.com";
	context.state = SMTPStates::POST_DATA;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("503 Bad sequence of commands\r\n").c_str(),
		response.c_str());
}

TEST(SMTPCommandsTest, RCPTNoArgTest)
{
	RCPTCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	//args.arguments["forward_path"] = "forward@path.com";
	context.state = SMTPStates::POST_RCPT;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("501 Syntax error in parameters or arguments\r\n").c_str(),
		response.c_str());
}

TEST(SMTPCommandsTest, RCPTBadArgTest)
{
	RCPTCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	args.arguments["forward_path"] = "not mail address";
	context.state = SMTPStates::POST_RCPT;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("553 Requested action not taken: mailbox syntax is incorrect\r\n").c_str(),
		response.c_str());
}

TEST(SMTPCommandsTest, NOOPTest)
{
	NOOPCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("250 Action completed\r\n").c_str(),
		response.c_str());
}

TEST(SMTPCommandsTest, DATATest)
{
	DATACommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	context.state = SMTPStates::POST_RCPT;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("354 Start mail input; end with <CRLF>.<CRLF>\r\n").c_str(),
		response.c_str());
	ASSERT_EQ(context.state, SMTPStates::POST_DATA);
}

TEST(SMTPCommandsTest, DATABadSequenceTest)
{
	DATACommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	context.state = SMTPStates::POST_MAIL;
	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("503 Bad sequence of commands\r\n").c_str(),
		response.c_str());
}

TEST(SMTPCommandsTest, RSETTest)
{
	RSETCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	context.state = SMTPStates::POST_RCPT;
	context.reverse_path.Append("some data");
	context.forward_path.Append("some data");
	context.mail_data.Append("some data");

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("250 Action completed\r\n").c_str(),
		response.c_str());
	ASSERT_EQ(context.state, SMTPStates::POST_EHLO);
	ASSERT_STREQ(context.forward_path.GetString().c_str(), "");
	ASSERT_STREQ(context.reverse_path.GetString().c_str(), "");
	ASSERT_STREQ(context.mail_data.GetString().c_str(), "");
}

TEST(SMTPCommandsTest, QUITTest)
{
	QUITCommand command;
	SMTPContext context;
	SMTPCommandArguments args(context, {}, nullptr);
	context.state = SMTPStates::POST_RCPT;

	auto reply = command.Invoke(args);
	std::string response;
	for (auto i : reply)
	{
		response.append_range(i.ToString());
	}

	ASSERT_STREQ(std::string("221 " + ISXSMTP::g_ServerDomain + " Service closing transmission channel\r\n").c_str(),
		response.c_str());
	ASSERT_EQ(context.state, SMTPStates::FINISH);
}