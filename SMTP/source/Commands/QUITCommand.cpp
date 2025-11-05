#include "Commands/QUITCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments)
{
	arguments.context.state.Set(SMTPStates::FINISH);

	return { SMTPReply::ServiceClosing(arguments.domain) };
}

std::string ISXSMTP::QUITCommand::GetName()
{
	return "QUIT";
}

std::string ISXSMTP::QUITCommand::GetSyntax()
{
	return  std::string("QUIT") + SMTPConstants::CR + SMTPConstants::LF;
}
