#include "Commands/EHLOCommand.h"
#include "SMTPConstants.h"

ISXSMTP::EHLOCommand::EHLOCommand()
{

}

std::vector<ISXSMTP::SMTPReply> ISXSMTP::EHLOCommand::Invoke(SMTPCommandArguments arguments)
{
	if (arguments.context->state == SMTPStates::FINISH)
		return { SMTPReply::BadSequenceOfCommands() };

	SMTPString domain;
	try
	{
		domain = arguments.arguments.at("domain");
	}
	catch (...)
	{
		return { SMTPReply::SyntaxError() };
	}

	arguments.context->state = SMTPStates::POST_EHLO;

	return {
		SMTPReply(250, SMTPString("<domain> greets ") + domain, true),
		SMTPReply(250, SMTPString("HELP"))
	};
}

ISXSMTP::SMTPString ISXSMTP::EHLOCommand::GetName()
{
	return SMTPString("EHLO");
}

ISXSMTP::SMTPString ISXSMTP::EHLOCommand::GetSyntax()
{
	return SMTPString("EHLO") + SMTPConstants::SP + SMTPString("!domain!") + SMTPConstants::CR + SMTPConstants::LF;
}
