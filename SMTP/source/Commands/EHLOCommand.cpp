#include "Commands/EHLOCommand.h"
#include "SMTPConstants.h"

ISXSMTP::EHLOCommand::EHLOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::EHLOCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
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
