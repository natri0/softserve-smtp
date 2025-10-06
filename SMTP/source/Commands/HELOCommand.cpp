#include "Commands/HELOCommand.h"

ISXSMTP::HELOCommand::HELOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELOCommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::HELOCommand::GetName() const
{
	return std::string("HELO");
}

