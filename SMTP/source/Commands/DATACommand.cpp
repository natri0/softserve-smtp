#include "Commands/DATACommand.h"

ISXSMTP::DATACommand::DATACommand()
{

}

ISXSMTP::SMTPCommandResult ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::DATACommand::GetName() const
{
	return std::string("DATA");
}

