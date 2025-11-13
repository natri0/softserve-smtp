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
	return "NOOP";
}

std::string ISXSMTP::NOOPCommand::GetSyntax()
{
	return  std::string("NOOP [string]") + SMTPConstants::CR + SMTPConstants::LF;
}
