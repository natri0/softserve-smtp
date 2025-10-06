#include "Commands/EHLOCommand.h"

ISXSMTP::EHLOCommand::EHLOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::EHLOCommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::EHLOCommand::GetName() const
{
	return std::string("EHLO");
}

