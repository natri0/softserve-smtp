#include "Commands/RSETCommand.h"
#include "SMTPConstants.h"

ISXSMTP::RSETCommand::RSETCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RSETCommand::Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context)
{
	context->forward_path.Clear();
	context->reverse_path.Clear();
	context->mail_data.Clear();
	context->state.Set(SMTPStates::INITIAL);

	return SMTPReply::OK();
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetName()
{
	return SMTPString("RSET");
}

ISXSMTP::SMTPString ISXSMTP::RSETCommand::GetSyntax()
{
	return SMTPString("RSET") + SMTPConstants::CR + SMTPConstants::LF;;
}
