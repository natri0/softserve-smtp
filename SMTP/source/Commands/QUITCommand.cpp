#include "Commands/QUITCommand.h"
#include "SMTPConstants.h"
#include "SMTPSession.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments)
{
	arguments.context.state.Set(SMTPStates::FINISH);

	return { SMTPReply::ServiceClosing(ISXSMTP::SMTPSession::GetDomain()) };
}

std::string ISXSMTP::QUITCommand::GetName()
{
	return "QUIT";
}

std::string ISXSMTP::QUITCommand::GetSyntax()
{
	return  std::string("QUIT") + SMTPConstants::CR + SMTPConstants::LF;
}
