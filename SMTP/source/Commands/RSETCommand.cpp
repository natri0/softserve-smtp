#include "Commands/RSETCommand.h"
#include "SMTPConstants.h"

std::vector<ISXSMTP::SMTPReply> ISXSMTP::RSETCommand::Invoke(SMTPCommandArguments arguments)
{
	arguments.context.forward_path.Clear();
	arguments.context.reverse_path.Clear();
	arguments.context.mail_data.Clear();
	arguments.context.state.Set(SMTPStates::INITIAL);

	return { SMTPReply::OK() };
}

std::string ISXSMTP::RSETCommand::GetName()
{
	return "RSET";
}

std::string ISXSMTP::RSETCommand::GetSyntax()
{
	return  std::string("RSET") + SMTPConstants::CR + SMTPConstants::LF;
}
