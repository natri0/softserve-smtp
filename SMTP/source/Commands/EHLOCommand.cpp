#include "Commands/EHLOCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::EHLOCommand::Invoke(SMTPCommandArguments arguments)
{
	if (arguments.context.state == SMTPStates::FINISH)
		return { SMTPReply::BadSequenceOfCommands() };

	std::string domain;
	try
	{
		domain = arguments.arguments.at("domain");
	}
	catch (...)
	{
		return { SMTPReply::SyntaxError() };
	}

	arguments.context.state = SMTPStates::POST_EHLO;

	return {
		SMTPReply(250, arguments.domain + " greets " + domain, true),
		SMTPReply(250, std::string("HELP"))
	};
}

std::string ISXSMTP::EHLOCommand::GetName()
{
	return "EHLO";
}

std::string ISXSMTP::EHLOCommand::GetSyntax()
{
	return std::string("EHLO !domain!") + SMTPConstants::CR + SMTPConstants::LF;
}
