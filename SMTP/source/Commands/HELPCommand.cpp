#include "Commands/HELPCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::HELPCommand::Invoke(SMTPCommandArguments arguments)
{
	return { SMTPReply::CommandNotImplemented() };
}

std::string ISXSMTP::HELPCommand::GetName()
{
	return "HELP";
}

std::string ISXSMTP::HELPCommand::GetSyntax()
{
	return  std::string("HELP [string]") + SMTPConstants::CR + SMTPConstants::LF;
}
