#include "Commands/MAILCommand.h"
#include "SMTPConstants.h"

#include <regex>

std::vector<ISXSMTP::SMTPReply> ISXSMTP::MAILCommand::Invoke(SMTPCommandArguments arguments)
{
	if (!arguments.context.is_authenticated) {
		return { SMTPReply(530, "Authentication required") };
	}
	if (arguments.context.state == SMTPStates::POST_RCPT || // mail command cannot be called when another transaction is in progress
		arguments.context.state == SMTPStates::INITIAL   || // mail should be called after ehlo or helo command
		arguments.context.state == SMTPStates::POST_DATA || // mail command cannot be called when another transaction is in progress
		arguments.context.state == SMTPStates::FINISH)		// mail command cannot be called after quit 
		return { SMTPReply::BadSequenceOfCommands() };

	std::string reverse_path;
	try
	{
		reverse_path = arguments.arguments.at("reverse_path");
	}
	catch (...)
	{
		reverse_path = "";
		//return { SMTPReply::SyntaxError() };
	}

	// check if forward_path is valid mail address
	if (!reverse_path.empty())
	{ 
		static std::regex mail_pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
		if (!std::regex_match(reverse_path, mail_pattern))
			return { SMTPReply::MailboxSyntaxIncorrect() };
	}

	arguments.context.forward_path.Clear();
	arguments.context.reverse_path.Clear();
	arguments.context.mail_data.Clear();
					 
	arguments.context.reverse_path.Append(reverse_path);
					 
	arguments.context.state.Set(SMTPStates::POST_MAIL);

	return { SMTPReply::OK() };
}

std::string ISXSMTP::MAILCommand::GetName()
{
	return std::string("MAIL");
}

std::string ISXSMTP::MAILCommand::GetSyntax()
{
	// mail command should also accept mail params but since 
	// I couldn't find a singe example what this params could mean or do
	// I decided to omit them for now
	// Specifying mail param will invoke Syntax Error, should be fixed in future
	return  std::string("MAIL FROM:<[reverse_path]>") + SMTPConstants::CR + SMTPConstants::LF;
}
