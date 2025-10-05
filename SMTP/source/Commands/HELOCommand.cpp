#include "Commands/HELOCommand.h"

ISXSMTP::HELOCommand::HELOCommand()
{

}

ISXSMTP::SMTPCommandResult ISXSMTP::HELOCommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::HELOCommand::GetName() const
{
	return std::string("HELO");
}

