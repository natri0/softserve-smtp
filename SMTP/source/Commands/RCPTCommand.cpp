#include "Commands/RCPTCommand.h"
#include "SMTPConstants.h"

#include <regex>

std::vector<ISXSMTP::SMTPReply> ISXSMTP::RCPTCommand::Invoke(SMTPCommandArguments arguments)
{
	if (!arguments.context.is_authenticated)
	{
		return { SMTPReply(530, "Authentication required") };
	}
	if (arguments.context.state != SMTPStates::POST_MAIL && arguments.context.state != SMTPStates::POST_RCPT)
		return { SMTPReply::BadSequenceOfCommands() };

	std::string forward_path;
	try
	{
		forward_path = arguments.arguments.at("forward_path");
	}
	catch (...)
	{
		return { SMTPReply::SyntaxError() };
	}

	// check if forward_path is valid mail address
	static std::regex mail_pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
	if (!std::regex_match(forward_path, mail_pattern))
		return { SMTPReply::MailboxSyntaxIncorrect() };
		
	// then we check if this address is from our domain
	auto snail_pos = forward_path.find_first_of('@');
	std::string domain = std::string(forward_path.begin() + snail_pos + 1, forward_path.end());
	if (domain != arguments.domain)
		return { SMTPReply::MailboxUnavailable550() };

	if (arguments.mailbox != nullptr) 
	{
		if (!arguments.mailbox->IsMailboxAvailable(forward_path))
		{
			return { SMTPReply::MailboxUnavailable550() };
		}
	}

	// there can be multiple recipients
	// so we add a delimiter used to distinguish them
	arguments.context.forward_path.Append(forward_path + ";");
	
	arguments.context.state = SMTPStates::POST_RCPT;

	return { SMTPReply::OK() };
}

std::string ISXSMTP::RCPTCommand::GetName()
{
	return "RCPT";
}

std::string ISXSMTP::RCPTCommand::GetSyntax()
{
	return  std::string("RCPT TO:<!forward_path!>") + SMTPConstants::CR + SMTPConstants::LF;;
}
