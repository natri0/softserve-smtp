#include "Commands/QUITCommand.h"

ISXSMTP::QUITCommand::QUITCommand()
{

}

ISXSMTP::SMTPCommandResult ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::QUITCommand::GetName() const
{
	return std::string("QUIT");
}

