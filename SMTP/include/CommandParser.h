#pragma once

#include "Commands/SMTPCommandArguments.h"
#include "Commands/SMTPCommandBase.h"
#include "SMTPString.h"

#include <vector>

namespace ISXSMTP
{

struct CommandParserResult
{
	SMTPCommandArguments parsed_arguments;

	// should only return syntax related error codes
	SMTPReply error_code;
};

class CommandParser
{
public:
	// static class
	CommandParser() = delete; 

	static CommandParserResult Parse(
			SMTPString command,
			const std::unordered_map<SMTPString, 
			std::unique_ptr<SMTPCommandBase>>& commands);

private:
	static size_t findCommandVerbIndex(const SMTPString& command);
	static SMTPString readArgName(const SMTPString& command_syntax, size_t& start);
	static SMTPString readArgValue(const SMTPString& command, size_t& start);
	static void handleMandatoryArgument(
			const SMTPString& command, size_t& command_index,
			const SMTPString& command_syntax, size_t& syntax_index,
			CommandParserResult& result);
	static void handleOptionalArgument(
		const SMTPString& command, size_t& command_index,
		const SMTPString& command_syntax, size_t& syntax_index,
		CommandParserResult& result);
	static void handleSyntaxDeviation(
		const SMTPString& command, size_t& command_index,
		const SMTPString& command_syntax, size_t& syntax_index,
		CommandParserResult& result);
};

}
