#include "Commands/RSETCommand.h"

ISXSMTP::RSETCommand::RSETCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RSETCommand::Invoke(SMTPCommandArguments arguments)
{
	return ISXSMTP::SMTPReply::CommandNotImplemented();
}

std::string ISXSMTP::RSETCommand::GetName() const
{
	return std::string("RSET");
}
