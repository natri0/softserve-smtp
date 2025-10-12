#include "Commands/RSETCommand.h"
#include "SMTPConstants.h"

ISXSMTP::RSETCommand::RSETCommand()
{

}

std::vector<ISXSMTP::SMTPReply> ISXSMTP::RSETCommand::Invoke(SMTPCommandArguments arguments)
{
	arguments.context->forward_path.Clear();
	arguments.context->reverse_path.Clear();
	arguments.context->mail_data.Clear();
	arguments.context->state.Set(SMTPStates::INITIAL);

	return { SMTPReply::OK() };
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetName()
{
	return SMTPString("RSET");
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetSyntax()
{
	return SMTPString("RSET") + SMTPConstants::CR + SMTPConstants::LF;;
}
