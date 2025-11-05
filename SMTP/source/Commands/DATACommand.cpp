#include "Commands/DATACommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments)
{
	if (arguments.context.state != SMTPStates::POST_RCPT)
		return { SMTPReply::BadSequenceOfCommands() };
	arguments.context.state = SMTPStates::POST_DATA;
	return { SMTPReply::StartMailInput() };
}

std::string ISXSMTP::DATACommand::GetName()
{
	return "DATA";
}

std::string ISXSMTP::DATACommand::GetSyntax()
{
	return std::string("DATA") + SMTPConstants::CR + SMTPConstants::LF;
}
