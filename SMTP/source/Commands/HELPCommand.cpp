#include "Commands/HELPCommand.h"



ISXSMTP::HELPCommand::HELPCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELPCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::HELPCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::HELPCommand::GetSyntax()
{
	return {};
}
