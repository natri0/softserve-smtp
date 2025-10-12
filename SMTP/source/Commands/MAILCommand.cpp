#include "Commands/MAILCommand.h"
#include "SMTPConstants.h"

ISXSMTP::MAILCommand::MAILCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::MAILCommand::Invoke(SMTPCommandArguments arguments)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::MAILCommand::GetName()
{
	return SMTPString("MAIL");
}

ISXSMTP::SMTPString ISXSMTP::MAILCommand::GetSyntax()
{
	// mail command should also accept mail params but since 
	// I couldn't find a singe example what this params could mean or do
	// I decided to omit them for now
	// Specifying mail param will invoke Syntax Error, should be fixed in future
	return SMTPString("MAIL") + SMTPConstants::SP + SMTPString("FROM:") + SMTPString("<!reverse_path!>") + SMTPConstants::CR + SMTPConstants::LF;
}
