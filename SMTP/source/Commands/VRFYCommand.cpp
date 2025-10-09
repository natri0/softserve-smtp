#include "Commands/VRFYCommand.h"

ISXSMTP::VRFYCommand::VRFYCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::VRFYCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::VRFYCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::VRFYCommand::GetSyntax()
{
	return {};
}
