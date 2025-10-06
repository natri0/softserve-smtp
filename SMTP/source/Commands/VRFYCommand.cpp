#include "Commands/VRFYCommand.h"

ISXSMTP::VRFYCommand::VRFYCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::VRFYCommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::VRFYCommand::GetName() const
{
	return std::string("VRFY");
}

