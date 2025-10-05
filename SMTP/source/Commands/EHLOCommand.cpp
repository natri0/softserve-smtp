#include "Commands/EHLOCommand.h"

ISXSMTP::EHLOCommand::EHLOCommand()
{

}

ISXSMTP::SMTPCommandResult ISXSMTP::EHLOCommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::EHLOCommand::GetName() const
{
	return std::string("EHLO");
}

