#include "Commands/RCPTCommand.h"
#include "SMTPConstants.h"

ISXSMTP::RCPTCommand::RCPTCommand()
{

}

std::vector<ISXSMTP::SMTPReply> ISXSMTP::RCPTCommand::Invoke(SMTPCommandArguments arguments)
{
	if (arguments.context->state != SMTPStates::POST_MAIL && arguments.context->state != SMTPStates::POST_RCPT)
		return { SMTPReply::BadSequenceOfCommands() };

	SMTPString forward_path;
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
	// so delimiter uses to distinguish them
	arguments.context->forward_path.Append(forward_path + ";");
	
	arguments.context->state = SMTPStates::POST_RCPT;

	return { SMTPReply::OK() };
}

ISXSMTP::SMTPString ISXSMTP::RCPTCommand::GetName()
{
	return SMTPString("RCPT");
}

ISXSMTP::SMTPString ISXSMTP::RCPTCommand::GetSyntax()
{
	return SMTPString("RCPT") + SMTPConstants::SP + SMTPString("TO:") + SMTPString("<!forward_path!>") + SMTPConstants::CR + SMTPConstants::LF;;
}
