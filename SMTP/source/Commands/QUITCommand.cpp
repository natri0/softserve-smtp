#include "Commands/QUITCommand.h"
#include "SMTPConstants.h"

ISXSMTP::QUITCommand::QUITCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::QUITCommand::GetName()
{
	return "QUIT";
}

ISXSMTP::SMTPString ISXSMTP::QUITCommand::GetSyntax()
{
	return SMTPString("QUIT") + SMTPConstants::CR + SMTPConstants::LF;
}
