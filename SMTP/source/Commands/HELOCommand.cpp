#include "Commands/HELOCommand.h"
#include "SMTPConstants.h"

ISXSMTP::HELOCommand::HELOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELOCommand::Invoke(SMTPCommandArguments arguments)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::HELOCommand::GetName()
{
	return "HELO";
}

ISXSMTP::SMTPString ISXSMTP::HELOCommand::GetSyntax()
{
	return SMTPString("HELO") + SMTPConstants::SP + SMTPString("!domain!") + SMTPConstants::CR + SMTPConstants::LF;
}
