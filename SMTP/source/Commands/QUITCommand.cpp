#include "Commands/QUITCommand.h"

ISXSMTP::QUITCommand::QUITCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::QUITCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::QUITCommand::GetSyntax()
{
	return {};
}
