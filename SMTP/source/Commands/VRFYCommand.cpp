#include "Commands/VRFYCommand.h"
#include "SMTPConstants.h"
#include "Profiler.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::VRFYCommand::Invoke(SMTPCommandArguments arguments)
{
	PROFILE_FUNC();
	return { SMTPReply::CommandNotImplemented() };
}

std::string ISXSMTP::VRFYCommand::GetName()
{
	PROFILE_FUNC();
	return "VRFY";
}

std::string ISXSMTP::VRFYCommand::GetSyntax()
{
	PROFILE_FUNC();
	return  std::string("VRFY !string!") + SMTPConstants::CR + SMTPConstants::LF;
}
