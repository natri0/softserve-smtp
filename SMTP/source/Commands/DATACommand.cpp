#include "Commands/DATACommand.h"

ISXSMTP::DATACommand::DATACommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::DATACommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::DATACommand::GetName() const
{
	return std::string("DATA");
}

