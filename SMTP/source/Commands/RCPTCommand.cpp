#include "Commands/RCPTCommand.h"


ISXSMTP::RCPTCommand::RCPTCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RCPTCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::RCPTCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::RCPTCommand::GetSyntax()
{
	return {};
}
