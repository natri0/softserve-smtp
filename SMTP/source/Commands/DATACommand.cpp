#include "Commands/DATACommand.h"
#include "SMTPConstants.h"

ISXSMTP::DATACommand::DATACommand()
{

}

std::vector<ISXSMTP::SMTPReply> ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments)
{
	//if (arguments.context->state != SMTPStates::POST_RCPT)
		//return { SMTPReply::BadSequenceOfCommands() };
	arguments.context->state = SMTPStates::POST_DATA;
	return { SMTPReply::StartMailInput() };
}

ISXSMTP::SMTPString ISXSMTP::DATACommand::GetName()
{
	return "DATA";
}

ISXSMTP::SMTPString ISXSMTP::DATACommand::GetSyntax()
{
	return SMTPString("DATA") + SMTPConstants::CR + SMTPConstants::LF;
}
