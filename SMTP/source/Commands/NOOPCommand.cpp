#include "Commands/NOOPCommand.h"
#include "SMTPConstants.h"
#include "Profiler.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::NOOPCommand::Invoke(SMTPCommandArguments arguments)
{
	PROFILE_FUNC();
	return { SMTPReply::OK() };
}

std::string ISXSMTP::NOOPCommand::GetName()
{
	PROFILE_FUNC();
	return "NOOP";
}

std::string ISXSMTP::NOOPCommand::GetSyntax()
{
	PROFILE_FUNC();
	return  std::string("NOOP [string]") + SMTPConstants::CR + SMTPConstants::LF;
}
