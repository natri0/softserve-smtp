#include "Commands/HELPCommand.h"
#include "SMTPConstants.h"
#include "Profiler.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::HELPCommand::Invoke(SMTPCommandArguments arguments)
{
	PROFILE_FUNC();
	return { SMTPReply::CommandNotImplemented() };
}

std::string ISXSMTP::HELPCommand::GetName()
{
	PROFILE_FUNC();
	return "HELP";
}

std::string ISXSMTP::HELPCommand::GetSyntax()
{
	PROFILE_FUNC();
	return  std::string("HELP [string]") + SMTPConstants::CR + SMTPConstants::LF;
}
