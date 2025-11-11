#include "Commands/HELOCommand.h"
#include "SMTPConstants.h"
#include "Profiler.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::HELOCommand::Invoke(SMTPCommandArguments arguments)
{
	PROFILE_FUNC();
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

	std::string comment = std::string(arguments.domain + " greets ") + domain;
	return { SMTPReply(250, comment) };
}

std::string ISXSMTP::HELOCommand::GetName()
{
	PROFILE_FUNC();
	return "HELO";
}

std::string ISXSMTP::HELOCommand::GetSyntax()
{
	PROFILE_FUNC();
	return std::string("HELO !domain!") + SMTPConstants::CR + SMTPConstants::LF;
}
