#pragma once

#include "Commands/SMTPCommandArguments.h"
#include "Commands/SMTPCommandBase.h"
#include "SMTPString.h"

#include <vector>

namespace ISXSMTP
{

/**
 * @brief Struct that contains result of SMTPCommandParser
 */
struct SMTPCommandParserResult
{
	/**
	 * @brief First word of the command denoted by <SP>
	 */
	SMTPString command_verb;

	/**
	 * @brief SMTPCommandParser fill map with arguments passed with the command
	 *	Context and mailbox left untouched
	 */
	SMTPCommandArguments parsed_arguments;

	/**
	 * @brief If syntax error occurs SMTPCommandParser writes it's code to this variable
	 */
	SMTPReply error_code;
};

/**
 * @brief Static class that parses SMTP command
 */
class SMTPCommandParser
{
public:
	/**
	 * @brief Default constructor deleted to forbid creation instances of this class
	 */
	SMTPCommandParser() = delete;
	
	/**
	 * @brief Parses SMTP command. SMTP command are case insensitive
	 * @param command SMTPString that contains command
	 * @param commands std::unordered_map with command available in the SMTPSession
	 * @return SMTPCommandParserResult 
	 */
	static SMTPCommandParserResult Parse(
			SMTPString command,
			const std::unordered_map<SMTPString, std::unique_ptr<SMTPCommandBase>>& commands);

private:
	/**
	 * @brief Parses command verb from the given command
	 * @param command SMTPString
	 * @return index in the command where command verb ends
	 */
	static size_t findCommandVerbIndex(const SMTPString& command);

	/**
	 * @brief reads argument name from the command syntax
	 * @param command_syntax SMTPString
	 * @param start index where argument name starts
	 * @return SMTPString with argument name
	 */
	static SMTPString readArgName(const SMTPString& command_syntax, size_t& start);

	/**
	 * @brief reads argument value from the command
	 * @param command SMTPString
	 * @param start index where argument value starts
	 * @return SMTPString with argument value
	 */
	static SMTPString readArgValue(const SMTPString& command, size_t& start);

	/**
	 * @brief Handles parsing of mandatory arguments
	 * @param command SMTPString
	 * @param command_index size_t&
	 * @param command_syntax SMTPString
	 * @param syntax_index size_t& 
	 * @param result SMTPCommandParserResult&
	 */
	static void handleMandatoryArgument(
			const SMTPString& command, size_t& command_index,
			const SMTPString& command_syntax, size_t& syntax_index,
		SMTPCommandParserResult& result);

	/**
	 * @brief Handles parsing of optional arguments
	 * @param command SMTPString
	 * @param command_index size_t&
	 * @param command_syntax SMTPString
	 * @param syntax_index size_t
	 * @param result SMTPCommandParserResult&
	 */
	static void handleOptionalArgument(
		const SMTPString& command, size_t& command_index,
		const SMTPString& command_syntax, size_t& syntax_index,
		SMTPCommandParserResult& result);

	/**
	 * @brief Handles case when command deviates from command syntax
	 * @param command SMTPString
	 * @param command_index size_t&
	 * @param command_syntax SMTPString
	 * @param syntax_index size_t&
	 * @param result SMTPCommandParserResult&
	 */
	static void handleSyntaxDeviation(
		const SMTPString& command, size_t& command_index,
		const SMTPString& command_syntax, size_t& syntax_index,
		SMTPCommandParserResult& result);
};

}
