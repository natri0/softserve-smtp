#include "Commands/MAILCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::MAILCommand::Invoke(SMTPCommandArguments arguments)
{
	// mail command should be called after ehlo or helo
	if (arguments.context->state == SMTPStates::FINISH || arguments.context->state == SMTPStates::INITIAL)
		return { SMTPReply::BadSequenceOfCommands() };

	SMTPString reverse_path;
	try
	{
		reverse_path = arguments.arguments.at("reverse_path");
	}
	catch (...)
	{
		return { SMTPReply::SyntaxError() };
	}

	arguments.context->forward_path.Clear();
	arguments.context->reverse_path.Clear();
	arguments.context->mail_data.Clear();

	arguments.context->reverse_path.Append(reverse_path);

	arguments.context->state.Set(SMTPStates::POST_MAIL);

	return { SMTPReply::OK() };
}

ISXSMTP::SMTPString ISXSMTP::MAILCommand::GetName()
{
	return SMTPString("MAIL");
}

ISXSMTP::SMTPString ISXSMTP::MAILCommand::GetSyntax()
{
	// mail command should also accept mail params but since 
	// I couldn't find a singe example what this params could mean or do
	// I decided to omit them for now
	// Specifying mail param will invoke Syntax Error, should be fixed in future
	return SMTPString("MAIL") + SMTPConstants::SP + SMTPString("FROM:") + SMTPString("<!reverse_path!>") + SMTPConstants::CR + SMTPConstants::LF;
}
