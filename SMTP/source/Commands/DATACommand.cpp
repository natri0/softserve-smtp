#include "Commands/DATACommand.h"

ISXSMTP::DATACommand::DATACommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::DATACommand::GetName() const
{
	return std::string("DATA");
}

