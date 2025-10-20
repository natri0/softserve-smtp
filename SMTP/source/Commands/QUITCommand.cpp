#include "Commands/QUITCommand.h"
#include "SMTPConstants.h"
#include "SMTPDomain.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments)
{
	arguments.context.state.Set(SMTPStates::FINISH);

	return { SMTPReply::ServiceClosing(ISXSMTP::g_ServerDomain) };
}

std::string ISXSMTP::QUITCommand::GetName()
{
	return "QUIT";
}

std::string ISXSMTP::QUITCommand::GetSyntax()
{
	return  std::string("QUIT") + SMTPConstants::CR + SMTPConstants::LF;
}
