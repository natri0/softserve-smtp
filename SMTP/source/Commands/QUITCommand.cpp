#include "Commands/QUITCommand.h"

ISXSMTP::QUITCommand::QUITCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments)
{
	return {};
}

std::string ISXSMTP::QUITCommand::GetName() const
{
	return std::string("QUIT");
}

