#include "Commands/DATACommand.h"

ISXSMTP::DATACommand::DATACommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::DATACommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::DATACommand::GetSyntax()
{
	return {};
}
