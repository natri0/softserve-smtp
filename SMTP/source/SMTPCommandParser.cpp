#include "SMTPCommandParser.h"
#include "SMTPConstants.h"

#include <iostream>
#include <algorithm>

ISXSMTP::SMTPCommandParserResult ISXSMTP::SMTPCommandParser::Parse(
		const std::string& command,
		const std::unordered_map<std::string, std::unique_ptr<SMTPCommandBase>>& commands)
{
	/*
	* 1. Find command verb
	* 2. Get command syntax
	* 3. Parse arguments and fill struct
	*/

	// First word in the command followed by the <SP> is a command verb
	size_t command_verb_index = findCommandVerbIndex(command);
	if (command_verb_index == std::string::npos)
	{
		return { {}, {}, SMTPReply::SyntaxError() };
	}

	std::string command_verb = std::string(command.begin(), command.begin() + command_verb_index);
	std::transform(command_verb.begin(), command_verb.end(), command_verb.begin(), ::toupper);
	std::string command_syntax;
	try
	{
		command_syntax = commands.at(command_verb)->GetSyntax();
	}
	catch (...)
	{
		return { {}, {}, SMTPReply::CommandUnrecognized() };
	}

	SMTPCommandParserResult result = { command_verb, {}, SMTPReply::OK() };
	for (size_t i = 0, j = 0; i < command.size() && j < command_syntax.size(); i++, j++)
	{
		if (command_syntax[j] == '!') // mandatory argument found
		{
			handleMandatoryArgument(command, i, command_syntax, j, result);
			if (result.error_code != SMTPReply::OK())
			{
				// error occurred
				return result;
			}
		}
		else if (command_syntax[j] == '[') // optional argument found
		{
			handleOptionalArgument(command, i, command_syntax, j, result);
			if (result.error_code != SMTPReply::OK())
			{
				// error occurred
				return result;
			}
		}
		else if (std::toupper(command[i]) != command_syntax[j])
		{
			handleSyntaxDeviation(command, i, command_syntax, j, result);
			if (result.error_code != SMTPReply::OK())
			{
				// error occurred
				return result;
			}
		}	
	}

	return result;
}

size_t ISXSMTP::SMTPCommandParser::findCommandVerbIndex(const std::string& command)
{
	size_t command_verb_index = command.find_first_of(SMTPConstants::SP);
	if (command_verb_index == std::string::npos)
	{
		// this could mean that command have no arguments
		// in this case string should end with <CLRF>

		command_verb_index = command.find_first_of(std::string({ SMTPConstants::CR, SMTPConstants::LF }));
		if (command_verb_index == std::string::npos)
		{
			return std::string::npos;
		}
	}
	return command_verb_index;
}

std::string ISXSMTP::SMTPCommandParser::readArgName(const std::string& command_syntax, size_t& start)
{
	std::string arg_name;
	std::uint8_t end_char = command_syntax[start];
	if (end_char == '[')
		end_char = ']';
	start++;
	while (command_syntax[start] != end_char)
	{
		if (start >= command_syntax.size())
		{
			// something wrong with command syntax
			return {};
		}

		arg_name.append(1, command_syntax[start]);
		start++;
	}
	return arg_name;
}

std::string ISXSMTP::SMTPCommandParser::readArgValue(const std::string& command, size_t & start)
{
	std::string arg_value;
	if (command[start] == '\"')
	{
		start++;
		while (command[start] != '\"')
		{
			if (start >= command.size())
			{
				return {};
			}

			arg_value.append(1, command[start]);
			start++;
		}
	}
	else
	{
		while (command[start] != SMTPConstants::SP && command[start] != '>')
		{
			if (start >= command.size())
			{
				return {};
			}

			if (command[start] == SMTPConstants::CR || command[start] == SMTPConstants::SP)
			{
				// arg value ended
				break;
			}

			arg_value.append(1, command[start]);
			start++;
		}
	}
	return arg_value;
}

void ISXSMTP::SMTPCommandParser::handleMandatoryArgument(
	const std::string& command,
	size_t& command_index,
	const std::string&
	command_syntax,
	size_t& syntax_index,
	SMTPCommandParserResult& result)
{
	// this means next section inside '!'...'!' contains argument name
	// and the next word in command should be written as value to that argument

	// 1. read arg name
	std::string arg_name = readArgName(command_syntax, syntax_index);
	if (arg_name.empty())
	{
		result.error_code = SMTPReply(541, "Service failed due to internal error");
		return;
	}		

	// 2. read arg value
	std::string arg_value = readArgValue(command, command_index);
	if (arg_value.empty())
	{
		result.error_code = SMTPReply::SyntaxError();
		return;
	}	

	result.parsed_arguments.arguments[arg_name] = arg_value;
	result.error_code = SMTPReply::OK();
}

void ISXSMTP::SMTPCommandParser::handleOptionalArgument(
		const std::string& command,
		size_t& command_index,
		const std::string& command_syntax,
		size_t& syntax_index,
		SMTPCommandParserResult& result)
{
	// this means next section inside '['...']' contains optional argument name
	// and the next word in command may be written as value to that argument

	// 1. read arg name
	std::string arg_name = readArgName(command_syntax, syntax_index	);
	if (arg_name.empty())
	{
		result.error_code = SMTPReply(541, "Service failed due to internal error");
		return;
	}	

	// 2. read arg value
	std::string arg_value = readArgValue(command, command_index);
	if (!arg_value.empty())
	{
		result.parsed_arguments.arguments[arg_name] = arg_value;
	}

	result.error_code = SMTPReply::OK();
}

void ISXSMTP::SMTPCommandParser::handleSyntaxDeviation(
		const std::string& command, 
		size_t& command_index,
		const std::string& command_syntax,
		size_t& syntax_index,
		SMTPCommandParserResult& result)
{
	if (command[command_index] == SMTPConstants::SP && command_syntax[syntax_index] != SMTPConstants::CR)
	{
		// current implementation will not tolerate trailing <SP> between arguments
		// this may be changed later
		result.error_code = SMTPReply::SyntaxError();
		return;
	}

	if (command[command_index] == SMTPConstants::CR && command_syntax[syntax_index] == SMTPConstants::SP)
	{
		// this means syntax have additional args that command doesn't specify
		// mandatory arguments in syntax are denoted with '!'
		// if syntax have one those that command doesn't specify 
		// parser should return syntax error

		// search for mandatory args in command syntax
		while (syntax_index < command_syntax.size())
		{
			if (command_syntax[syntax_index] == '!')
			{
				result.error_code = SMTPReply::SyntaxError();
				return;
			}
				
			syntax_index++;
		}
	}
}
