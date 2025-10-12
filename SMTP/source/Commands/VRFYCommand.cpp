#include "Commands/VRFYCommand.h"
#include "SMTPConstants.h"

ISXSMTP::VRFYCommand::VRFYCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::VRFYCommand::Invoke(SMTPCommandArguments arguments)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::VRFYCommand::GetName()
{
	return SMTPString("VRFY");
}

ISXSMTP::SMTPString ISXSMTP::VRFYCommand::GetSyntax()
{
	return SMTPString("VRFY") + SMTPConstants::SP + SMTPString("!string!") + SMTPConstants::CR + SMTPConstants::LF;
}
