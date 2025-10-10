#include "Commands/HELPCommand.h"
#include "SMTPConstants.h"

ISXSMTP::HELPCommand::HELPCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELPCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::HELPCommand::GetName()
{
	return SMTPString("HELP");
}

ISXSMTP::SMTPString ISXSMTP::HELPCommand::GetSyntax()
{
	return SMTPString("HELP") + SMTPConstants::SP + SMTPString("[string]") + SMTPConstants::CR + SMTPConstants::LF;
}
