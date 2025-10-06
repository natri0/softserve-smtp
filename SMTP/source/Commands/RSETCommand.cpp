#include "Commands/RSETCommand.h"

ISXSMTP::RSETCommand::RSETCommand()
{

}

ISXSMTP::SMTPReply ISXSMTP::RSETCommand::Invoke(SMTPCommandArguments arguments)
{
	arguments.forward_path.Clear();
	arguments.reverse_path.Clear();
	arguments.mail_data.Clear();
	arguments.state.Clear();

	return ISXSMTP::SMTPReply::OK();
}

std::string ISXSMTP::RSETCommand::GetName() const
{
	return std::string("RSET");
}
