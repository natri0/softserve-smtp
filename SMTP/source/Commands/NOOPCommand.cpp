#include "Commands/NOOPCommand.h"


ISXSMTP::NOOPCommand::NOOPCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::NOOPCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::NOOPCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::NOOPCommand::GetSyntax()
{
	return {};
}
