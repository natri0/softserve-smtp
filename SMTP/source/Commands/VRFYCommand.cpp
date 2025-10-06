#include "Commands/VRFYCommand.h"

ISXSMTP::VRFYCommand::VRFYCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::VRFYCommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::VRFYCommand::GetName() const
{
	return std::string("VRFY");
}

