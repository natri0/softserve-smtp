#include "Commands/MAILCommand.h"


ISXSMTP::MAILCommand::MAILCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::MAILCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::MAILCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::MAILCommand::GetSyntax()
{
	return {};
}
