#include "Commands/RSETCommand.h"
#include "SMTPConstants.h"

ISXSMTP::RSETCommand::RSETCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RSETCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetName()
{
	return SMTPString("RSET");
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetSyntax()
{
	return SMTPString("RSET") + SMTPConstants::CR + SMTPConstants::LF;;
}
