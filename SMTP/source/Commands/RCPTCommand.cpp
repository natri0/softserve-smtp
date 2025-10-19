#include "Commands/RCPTCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::RCPTCommand::Invoke(SMTPCommandArguments arguments)
{
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

	/*if (!arguments.mailbox->IsMailboxAvailable(forward_path))
		return { SMTPReply::MailboxUnavailable550() };*/

	// there can be multiple recipients
	// so delimiter used to distinguish them
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
