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
	return "VRFY";
}

std::string ISXSMTP::VRFYCommand::GetSyntax()
{
	return  std::string("VRFY !string!") + SMTPConstants::CR + SMTPConstants::LF;
}
