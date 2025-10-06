#include "Commands/NOOPCommand.h"

#include <iostream>

ISXSMTP::NOOPCommand::NOOPCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::NOOPCommand::Invoke(SMTPCommandArguments arguments)
{
	// this command does nothing
	return ISXSMTP::SMTPReply::OK();
}

std::string ISXSMTP::NOOPCommand::GetName() const
{
	return std::string("NOOP");
}

