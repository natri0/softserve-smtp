#include "Commands/QUITCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::QUITCommand::Invoke(SMTPCommandArguments arguments)
{
	arguments.context->state.Set(SMTPStates::FINISH);

	return { SMTPReply::ServiceClosing() };
}

ISXSMTP::SMTPString ISXSMTP::QUITCommand::GetName()
{
	return "QUIT";
}

ISXSMTP::SMTPString ISXSMTP::QUITCommand::GetSyntax()
{
	return SMTPString("QUIT") + SMTPConstants::CR + SMTPConstants::LF;
}
