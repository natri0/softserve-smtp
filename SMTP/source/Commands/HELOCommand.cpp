#include "Commands/HELOCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::HELOCommand::Invoke(SMTPCommandArguments arguments)
{
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

	std::string comment = std::string(arguments.context.domain + " greets ") + domain;
	return { SMTPReply(250, comment) };
}

std::string ISXSMTP::HELOCommand::GetName()
{
	return "HELO";
}

std::string ISXSMTP::HELOCommand::GetSyntax()
{
	return std::string("HELO !domain!") + SMTPConstants::CR + SMTPConstants::LF;
}
