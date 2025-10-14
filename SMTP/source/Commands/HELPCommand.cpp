#include "Commands/HELPCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::HELPCommand::Invoke(SMTPCommandArguments arguments)
{
	return { SMTPReply::CommandNotImplemented() };
}

ISXSMTP::SMTPString ISXSMTP::HELPCommand::GetName()
{
	return SMTPString("HELP");
}

ISXSMTP::SMTPString ISXSMTP::HELPCommand::GetSyntax()
{
	return SMTPString("HELP") + SMTPConstants::SP + SMTPString("[string]") + SMTPConstants::CR + SMTPConstants::LF;
}
