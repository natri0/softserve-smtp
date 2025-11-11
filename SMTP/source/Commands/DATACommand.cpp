#include "Commands/DATACommand.h"
#include "SMTPConstants.h"
#include "Profiler.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments)
{
	PROFILE_FUNC();
	if (arguments.context.state != SMTPStates::POST_RCPT)
		return { SMTPReply::BadSequenceOfCommands() };
	arguments.context.state = SMTPStates::POST_DATA;
	return { SMTPReply::StartMailInput() };
}

std::string ISXSMTP::DATACommand::GetName()
{
	PROFILE_FUNC();
	return "DATA";
}

std::string ISXSMTP::DATACommand::GetSyntax()
{
	PROFILE_FUNC();
	return std::string("DATA") + SMTPConstants::CR + SMTPConstants::LF;
}
