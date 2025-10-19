#pragma once

#include <gtest/gtest.h>

#include "SMTPCommandParser.h"
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

#include <unordered_map>
#include <memory>

using namespace ISXSMTP;

class SMTPCommandParserTest : public testing::Test
{
protected:
	void SetUp() override {
		auto EHLO = std::make_unique<EHLOCommand>();
		auto MAIL = std::make_unique<MAILCommand>();
		auto RCPT = std::make_unique<RCPTCommand>();
		auto DATA = std::make_unique<DATACommand>();
		auto HELP = std::make_unique<HELPCommand>();
		auto HELO = std::make_unique<HELOCommand>();
		auto RSET = std::make_unique<RSETCommand>();
		auto QUIT = std::make_unique<QUITCommand>();
		auto NOOP = std::make_unique<NOOPCommand>();
		auto VRFY = std::make_unique<VRFYCommand>();
		commands[EHLO->GetName()] = std::move(EHLO);
		commands[MAIL->GetName()] = std::move(MAIL);
		commands[RCPT->GetName()] = std::move(RCPT);
		commands[DATA->GetName()] = std::move(DATA);
		commands[HELP->GetName()] = std::move(HELP);
		commands[HELO->GetName()] = std::move(HELO);
		commands[RSET->GetName()] = std::move(RSET);
		commands[QUIT->GetName()] = std::move(QUIT);
		commands[NOOP->GetName()] = std::move(NOOP);
		commands[VRFY->GetName()] = std::move(VRFY);
	}

	void TearDown() override {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	std::unordered_map<std::string, std::unique_ptr<SMTPCommandBase>> commands;
};

TEST_F(SMTPCommandParserTest, EHLOBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("EHLO some@domain.com\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "EHLO");
	ASSERT_STREQ(parser_result.parsed_arguments["domain"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, EHLONoArgTest)
{
	auto parser_result = SMTPCommandParser::Parse("EHLO\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::SyntaxError().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "EHLO");
}

TEST_F(SMTPCommandParserTest, EHLOFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("eHlO some@domain.com\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "EHLO");
	ASSERT_STREQ(parser_result.parsed_arguments["domain"].c_str(), "some@domain.com");

	parser_result = SMTPCommandParser::Parse("EhLo some@domain.com\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "EHLO");
	ASSERT_STREQ(parser_result.parsed_arguments["domain"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, HELOBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("HELO some@domain.com\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELO");
	ASSERT_STREQ(parser_result.parsed_arguments["domain"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, HELONoArgTest)
{
	auto parser_result = SMTPCommandParser::Parse("HELO\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::SyntaxError().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELO");
}

TEST_F(SMTPCommandParserTest, HELOFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("helO some@domain.com\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELO");
	ASSERT_STREQ(parser_result.parsed_arguments["domain"].c_str(), "some@domain.com");

	parser_result = SMTPCommandParser::Parse("hElo some@domain.com\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELO");
	ASSERT_STREQ(parser_result.parsed_arguments["domain"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, DATABasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("DATA\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "DATA");
}

TEST_F(SMTPCommandParserTest, DATAUnexpectedArgTest)
{
	auto parser_result = SMTPCommandParser::Parse("DATA test\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "DATA");
}

TEST_F(SMTPCommandParserTest, DATAFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("dAtA\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "DATA");

	parser_result = SMTPCommandParser::Parse("Data\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "DATA");
}

TEST_F(SMTPCommandParserTest, MAILBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("MAIL FROM:<some@domain.com>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "MAIL");
	ASSERT_STREQ(parser_result.parsed_arguments["reverse_path"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, MAILNoArgTest)
{
	auto parser_result = SMTPCommandParser::Parse("MAIL FROM:<>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "MAIL");
}

TEST_F(SMTPCommandParserTest, MAILFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("mAil froM:<some@domain.com>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "MAIL");
	ASSERT_STREQ(parser_result.parsed_arguments["reverse_path"].c_str(), "some@domain.com");

	parser_result = SMTPCommandParser::Parse("Mail from:<some@domain.com>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "MAIL");
	ASSERT_STREQ(parser_result.parsed_arguments["reverse_path"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, RCPTBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("RCPT TO:<some@domain.com>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "RCPT");
	ASSERT_STREQ(parser_result.parsed_arguments["forward_path"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, RCPTNoArgTest)
{
	auto parser_result = SMTPCommandParser::Parse("RCPT TO:<>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::SyntaxError().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "RCPT");
}

TEST_F(SMTPCommandParserTest, RCPTFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("rCpT tO:<some@domain.com>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "RCPT");
	ASSERT_STREQ(parser_result.parsed_arguments["forward_path"].c_str(), "some@domain.com");

	parser_result = SMTPCommandParser::Parse("rcPt To:<some@domain.com>\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "RCPT");
	ASSERT_STREQ(parser_result.parsed_arguments["forward_path"].c_str(), "some@domain.com");
}

TEST_F(SMTPCommandParserTest, HELPBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("HELP some_string\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELP");
	ASSERT_STREQ(parser_result.parsed_arguments["string"].c_str(), "some_string");
}

TEST_F(SMTPCommandParserTest, HELPNoArgTest)
{
	auto parser_result = SMTPCommandParser::Parse("HELP\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELP");
}

TEST_F(SMTPCommandParserTest, HELPFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("HeLp some_string\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELP");
	ASSERT_STREQ(parser_result.parsed_arguments["string"].c_str(), "some_string");

	parser_result = SMTPCommandParser::Parse("hELP some_string\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "HELP");
	ASSERT_STREQ(parser_result.parsed_arguments["string"].c_str(), "some_string");
}

TEST_F(SMTPCommandParserTest, NOOPBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("NOOP\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "NOOP");
}

TEST_F(SMTPCommandParserTest, NOOPFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("nOOp\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "NOOP");

	parser_result = SMTPCommandParser::Parse("NOop\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "NOOP");
}

TEST_F(SMTPCommandParserTest, QUITBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("QUIT\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "QUIT");
}

TEST_F(SMTPCommandParserTest, QUITFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("qUit\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "QUIT");

	parser_result = SMTPCommandParser::Parse("QUiT\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "QUIT");
}

TEST_F(SMTPCommandParserTest, RSETBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("RSET\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "RSET");
}

TEST_F(SMTPCommandParserTest, RSETFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("RSet\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "RSET");

	parser_result = SMTPCommandParser::Parse("RsET\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "RSET");
}

TEST_F(SMTPCommandParserTest, VRFYBasicTest)
{
	auto parser_result = SMTPCommandParser::Parse("VRFY some_string\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "VRFY");
	ASSERT_STREQ(parser_result.parsed_arguments["string"].c_str(), "some_string");
}

TEST_F(SMTPCommandParserTest, VRFYNoArgTest)
{
	auto parser_result = SMTPCommandParser::Parse("VRFY\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::SyntaxError().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "VRFY");
}

TEST_F(SMTPCommandParserTest, VRFYFunnyCasesTest)
{
	auto parser_result = SMTPCommandParser::Parse("vrFy some_string\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "VRFY");
	ASSERT_STREQ(parser_result.parsed_arguments["string"].c_str(), "some_string");

	parser_result = SMTPCommandParser::Parse("VRfY some_string\r\n", commands);

	ASSERT_EQ(parser_result.error_code.GetCode(), SMTPReply::OK().GetCode());
	ASSERT_STREQ(parser_result.command_verb.c_str(), "VRFY");
	ASSERT_STREQ(parser_result.parsed_arguments["string"].c_str(), "some_string");
}
