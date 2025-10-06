#include "Commands/HELPCommand.h"

ISXSMTP::HELPCommand::HELPCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELPCommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::HELPCommand::GetName() const
{
	return std::string("HELP");
}

