#include "Commands/HELOCommand.h"

ISXSMTP::HELOCommand::HELOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::HELOCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::HELOCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::HELOCommand::GetSyntax()
{
	return {};
}
