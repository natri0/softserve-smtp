#include "Commands/MAILCommand.h"

ISXSMTP::MAILCommand::MAILCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::MAILCommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::MAILCommand::GetName() const
{
	return std::string("MAIL");
}

