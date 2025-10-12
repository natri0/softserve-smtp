#include "Commands/EHLOCommand.h"
#include "SMTPConstants.h"

ISXSMTP::EHLOCommand::EHLOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::EHLOCommand::Invoke(SMTPCommandArguments arguments)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::EHLOCommand::GetName()
{
	return SMTPString("EHLO");
}

ISXSMTP::SMTPString ISXSMTP::EHLOCommand::GetSyntax()
{
	return SMTPString("EHLO") + SMTPConstants::SP + SMTPString("!domain!") + SMTPConstants::CR + SMTPConstants::LF;
}
