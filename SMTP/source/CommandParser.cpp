#include "CommandParser.h"
#include "SMTPConstants.h"

#include <iostream>

ISXSMTP::CommandParserResult ISXSMTP::CommandParser::Parse(
		SMTPString command,
		const std::unordered_map<SMTPString, 
		std::unique_ptr<SMTPCommandBase>>& commands)
{
	/*
	* 1. Find command verb
	* 2. Get command syntax
	* 3. Parse arguments and fill struct
	*/

	// SMTP is case insensitive
	command.ToUpper();

	// First word in the command followed by the <SP> is a command verb
	size_t command_verb_index = findCommandVerbIndex(command);
	if (command_verb_index == SMTPString::NPOS)
	{
		return { {}, {}, SMTPReply::SyntaxError() };
	}

	SMTPString command_verb = command.GetRange(0, command_verb_index);
	SMTPString command_syntax;
	try
	{
		command_syntax = commands.at(command_verb)->GetSyntax();
	}
	catch (...)
	{
		return { {}, {}, SMTPReply::CommandUnrecognized() };
	}

	CommandParserResult result = { command_verb, {}, SMTPReply::OK() };
	for (size_t i = 0, j = 0; i < command.Count() && j < command_syntax.Count(); i++, j++)
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
		else if (command[i] != command_syntax[j])
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

size_t ISXSMTP::CommandParser::findCommandVerbIndex(const SMTPString& command)
{
	size_t command_verb_index = command.FindFirstOf(SMTPConstants::SP);
	if (command_verb_index == SMTPString::NPOS)
	{
		// this could mean that command have no arguments
		// in this case string should end with <CLRF>

		command_verb_index = command.FindFirstOf(SMTPString({ SMTPConstants::CR, SMTPConstants::LF }));
		if (command_verb_index == SMTPString::NPOS)
		{
			return SMTPString::NPOS;
		}
	}
	return command_verb_index;
}

ISXSMTP::SMTPString ISXSMTP::CommandParser::readArgName(const SMTPString& command_syntax, size_t& start)
{
	SMTPString arg_name;
	std::uint8_t end_char = command_syntax[start];
	if (end_char == '[')
		end_char = ']';
	start++;
	while (command_syntax[start] != end_char)
	{
		if (start >= command_syntax.Count())
		{
			// something wrong with command syntax
			return {};
		}

		arg_name.Append(command_syntax[start]);
		start++;
	}
	return arg_name;
}

ISXSMTP::SMTPString ISXSMTP::CommandParser::readArgValue(const SMTPString& command, size_t& start)
{
	SMTPString arg_value;
	if (command[start] == '\"')
	{
		start++;
		while (command[start] != '\"')
		{
			if (start >= command.Count())
			{
				return {};
			}

			arg_value.Append(command[start]);
			start++;
		}
	}
	else
	{
		while (command[start] != SMTPConstants::SP && command[start] != '>')
		{
			if (start >= command.Count())
			{
				return {};
			}

			if (command[start] == SMTPConstants::CR || command[start] == SMTPConstants::SP)
			{
				// arg value ended
				break;
			}

			arg_value.Append(command[start]);
			start++;
		}
	}
	return arg_value;
}

void ISXSMTP::CommandParser::handleMandatoryArgument(
	const SMTPString& command,
	size_t& command_index,
	const SMTPString&
	command_syntax,
	size_t& syntax_index,
	CommandParserResult& result)
{
	// this means next section inside '!'...'!' contains argument name
	// and the next word in command should be written as value to that argument

	// 1. read arg name
	SMTPString arg_name = readArgName(command_syntax, syntax_index);
	if (arg_name.IsEmpty())
	{
		result.error_code = SMTPReply({ 5, 4, 1 }, "Service failed due to internal error");
		return;
	}		

	// 2. read arg value
	SMTPString arg_value = readArgValue(command, command_index);
	if (arg_value.IsEmpty())
	{
		result.error_code = SMTPReply::SyntaxError();
		return;
	}	

	result.parsed_arguments.arguments[arg_name] = arg_value;
	result.error_code = SMTPReply::OK();
}

void ISXSMTP::CommandParser::handleOptionalArgument(
		const SMTPString& command,
		size_t& command_index,
		const SMTPString& command_syntax,
		size_t& syntax_index,
		CommandParserResult& result)
{
	// this means next section inside '['...']' contains optional argument name
	// and the next word in command may be written as value to that argument

	// 1. read arg name
	SMTPString arg_name = readArgName(command_syntax, syntax_index	);
	if (arg_name.IsEmpty())
	{
		result.error_code = SMTPReply({ 5, 4, 1 }, "Service failed due to internal error");
		return;
	}	

	// 2. read arg value
	SMTPString arg_value = readArgValue(command, command_index);
	if (!arg_value.IsEmpty())
	{
		result.parsed_arguments.arguments[arg_name] = arg_value;
	}

	result.error_code = SMTPReply::OK();
}

void ISXSMTP::CommandParser::handleSyntaxDeviation(
		const SMTPString& command, 
		size_t& command_index,
		const SMTPString& command_syntax,
		size_t& syntax_index,
		CommandParserResult& result)
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
		while (syntax_index < command_syntax.Count())
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
