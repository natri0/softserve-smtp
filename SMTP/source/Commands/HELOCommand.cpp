#include "Commands/HELOCommand.h"
#include "SMTPConstants.h"

ISXSMTP::HELOCommand::HELOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELOCommand::Invoke(SMTPCommandArguments arguments)
{
	SMTPString domain;
	try
	{
		domain = arguments.arguments.at("domain");
	}
	catch (...)
	{
		return SMTPReply::SyntaxError();
	}

	arguments.context->state = SMTPStates::POST_EHLO;

	SMTPString comment = SMTPString("<domain> greets ") + domain;
	return SMTPReply(250, comment);
}

ISXSMTP::SMTPString ISXSMTP::HELOCommand::GetName()
{
	return "HELO";
}

ISXSMTP::SMTPString ISXSMTP::HELOCommand::GetSyntax()
{
	return SMTPString("HELO") + SMTPConstants::SP + SMTPString("!domain!") + SMTPConstants::CR + SMTPConstants::LF;
}
