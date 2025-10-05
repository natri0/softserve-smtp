#include "Commands/MAILCommand.h"

ISXSMTP::MAILCommand::MAILCommand()
{

}

ISXSMTP::SMTPCommandResult ISXSMTP::MAILCommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::MAILCommand::GetName() const
{
	return std::string("MAIL");
}

