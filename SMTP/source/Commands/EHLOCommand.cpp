#include "Commands/EHLOCommand.h"



ISXSMTP::EHLOCommand::EHLOCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::EHLOCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	return SMTPReply::CommandNotImplemented();
}

ISXSMTP::SMTPString ISXSMTP::EHLOCommand::GetName()
{
	return {};
}

ISXSMTP::SMTPString ISXSMTP::EHLOCommand::GetSyntax()
{
	return {};
}
