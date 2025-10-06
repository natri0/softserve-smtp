#include "Commands/RCPTCommand.h"

ISXSMTP::RCPTCommand::RCPTCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RCPTCommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::RCPTCommand::GetName() const
{
	return std::string("RCPT");
}

