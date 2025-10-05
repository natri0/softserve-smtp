#include "Commands/NOOPCommand.h"

#include <iostream>

ISXSMTP::NOOPCommand::NOOPCommand()
{

}

ISXSMTP::SMTPCommandResult ISXSMTP::NOOPCommand::Invoke(SMTPCommandArguments arguments)
{
	std::cout << "Hello from NOOP command!\n";
	return {};
}

std::string ISXSMTP::NOOPCommand::GetName() const
{
	return std::string("NOOP");
}

