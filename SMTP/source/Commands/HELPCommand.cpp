#include "Commands/HELPCommand.h"

ISXSMTP::HELPCommand::HELPCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELPCommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::HELPCommand::GetName() const
{
	return std::string("HELP");
}

