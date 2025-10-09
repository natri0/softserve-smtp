#include "Commands/RSETCommand.h"

ISXSMTP::RSETCommand::RSETCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RSETCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetSyntax()
{
	return {};
}
