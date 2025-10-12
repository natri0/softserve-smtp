#include "Commands/RCPTCommand.h"
#include "SMTPConstants.h"

ISXSMTP::RCPTCommand::RCPTCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RCPTCommand::Invoke(SMTPCommandArguments arguments)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::RCPTCommand::GetName()
{
	return SMTPString("RCPT");
}

ISXSMTP::SMTPString ISXSMTP::RCPTCommand::GetSyntax()
{
	return SMTPString("RCPT") + SMTPConstants::SP + SMTPString("TO:") + SMTPString("<!forward_path!>") + SMTPConstants::CR + SMTPConstants::LF;;
}
