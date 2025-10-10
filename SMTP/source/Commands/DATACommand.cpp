#include "Commands/DATACommand.h"
#include "SMTPConstants.h"

ISXSMTP::DATACommand::DATACommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::DATACommand::GetName()
{
	return "DATA";
}

ISXSMTP::SMTPString ISXSMTP::DATACommand::GetSyntax()
{
	return SMTPString("DATA") + SMTPConstants::CR + SMTPConstants::LF;
}
