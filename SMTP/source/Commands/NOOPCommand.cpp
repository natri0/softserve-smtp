#include "Commands/NOOPCommand.h"
#include "SMTPConstants.h"

ISXSMTP::NOOPCommand::NOOPCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::NOOPCommand::Invoke(SMTPCommandArguments arguments)
{
	return SMTPReply::OK();
}

ISXSMTP::SMTPString ISXSMTP::NOOPCommand::GetName()
{
	return "NOOP";
}

ISXSMTP::SMTPString ISXSMTP::NOOPCommand::GetSyntax()
{
	return SMTPString("NOOP") + SMTPConstants::SP + SMTPString("[string]") + SMTPConstants::CR + SMTPConstants::LF;
}
