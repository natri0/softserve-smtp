#pragma once

#include "Commands/SMTPCommandBase.h"

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
	std::string command_verb;

	/**
	 * @brief SMTPCommandParser fill map with arguments passed with the command
	 *	Context and mailbox left untouched
	 */
	std::unordered_map<std::string, std::string> parsed_arguments;

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
	 * @param command std::string that contains command
	 * @param commands std::unordered_map with command available in the SMTPSession
	 * @return SMTPCommandParserResult 
	 */
	static SMTPCommandParserResult Parse(
			const std::string& command,
			const std::unordered_map<std::string, std::unique_ptr<SMTPCommandBase>>& commands);

private:
	/**
	 * @brief Parses command verb from the given command
	 * @param command std::string
	 * @return index in the command where command verb ends
	 */
	static size_t findCommandVerbIndex(const std::string& command);

	/**
	 * @brief reads argument name from the command syntax
	 * @param command_syntax std::string
	 * @param start index where argument name starts
	 * @return std::string with argument name
	 */
	static std::string readArgName(const std::string& command_syntax, size_t& start);

	/**
	 * @brief reads argument value from the command
	 * @param command std::string
	 * @param start index where argument value starts
	 * @return std::string with argument value
	 */
	static std::string readArgValue(const std::string& command, size_t& start);

	/**
	 * @brief Handles parsing of mandatory arguments
	 * @param command std::string
	 * @param command_index size_t&
	 * @param command_syntax std::string
	 * @param syntax_index size_t& 
	 * @param result SMTPCommandParserResult&
	 */
	static void handleMandatoryArgument(
			const std::string& command, size_t& command_index,
			const std::string& command_syntax, size_t& syntax_index,
		SMTPCommandParserResult& result);

	/**
	 * @brief Handles parsing of optional arguments
	 * @param command std::string
	 * @param command_index size_t&
	 * @param command_syntax std::string
	 * @param syntax_index size_t
	 * @param result SMTPCommandParserResult&
	 */
	static void handleOptionalArgument(
		const std::string& command, size_t& command_index,
		const std::string& command_syntax, size_t& syntax_index,
		SMTPCommandParserResult& result);

	/**
	 * @brief Handles case when command deviates from command syntax
	 * @param command std::string
	 * @param command_index size_t&
	 * @param command_syntax std::string
	 * @param syntax_index size_t&
	 * @param result SMTPCommandParserResult&
	 */
	static void handleSyntaxDeviation(
		const std::string& command, size_t& command_index,
		const std::string& command_syntax, size_t& syntax_index,
		SMTPCommandParserResult& result);
};

}
