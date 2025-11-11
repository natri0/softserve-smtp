#include "SMTPReply.h"
#include "Profiler.h"
#include "SMTPConstants.h"

ISXSMTP::SMTPReply::SMTPReply(std::uint16_t code, const std::string& comment, bool multi_line)
	: m_code(code)
	, m_comment(comment)
	, m_multiLine(multi_line)
{
	PROFILE_FUNC();
}

ISXSMTP::SMTPReply::SMTPReply(std::uint16_t code, const std::string& comment)
	: m_code(code)
	, m_comment(comment)
	, m_multiLine(false)
{
	PROFILE_FUNC();
}

std::string ISXSMTP::SMTPReply::GetComment() const
{
	PROFILE_FUNC();
	return m_comment;
}

std::uint16_t ISXSMTP::SMTPReply::GetCode() const
{
	PROFILE_FUNC();
	return m_code;
}

std::string ISXSMTP::SMTPReply::ToString() const
{
	PROFILE_FUNC();
	std::string result(std::to_string(m_code));
	if (m_multiLine)
		result.append("-");
	else
		result.append(" ");
	result.append(m_comment);
	result.append(1, SMTPConstants::CR);
	result.append(1, SMTPConstants::LF);
	return result;
}

void ISXSMTP::SMTPReply::SetMultiLine(bool value)
{
	PROFILE_FUNC();
	m_multiLine = value;
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CommandUnrecognized()
{
	return SMTPReply(500, "Command unrecognized");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::SyntaxError()
{
	return SMTPReply(501, "Syntax error in parameters or arguments");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CommandNotImplemented()
{
	return SMTPReply(502, "Command not implemented");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::BadSequenceOfCommands()
{
	return SMTPReply(503, "Bad sequence of commands");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CommandParameterNotImplemented()
{
	return SMTPReply(504, "Command parameter not implemented");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::HelpReply()
{
	return SMTPReply(211, "");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::HelpMessage()
{
	return SMTPReply(214, "");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ServiceReady(const std::string& domain)
{
	return SMTPReply(220, domain + " Service ready");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ServiceClosing(const std::string& domain)
{
	return SMTPReply(221, domain + " Service closing transmission channel");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ServiceNotAvailable()
{
	return SMTPReply(421, "Service not available, closing transmission channel");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::OK()
{
	return SMTPReply(250, "Action completed");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::UserNotLocal251()
{
	return SMTPReply(251, "User not local; will forward to <forward-path>");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::UserNotLocal551()
{
	return SMTPReply(551, "User not local; please try <forward-path>");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CannotVerifyUser()
{
	return SMTPReply(252, "Cannot verify user, but will accept message and try to delivery");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::MailboxUnavailable450()
{
	return SMTPReply(450, "Mailbox unavailable, try later");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::MailboxUnavailable550()
{
	return SMTPReply(550, "Mailbox unavailable");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ProcessingError()
{
	return SMTPReply(451, "Processing error, try later");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::InsufficientSystemStorage()
{
	return SMTPReply(452, "Insufficient system storage, try later");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ExceededStorageAllocation()
{
	return SMTPReply(552, "Exceeded storage allocation");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::StartMailInput()
{
	return SMTPReply(354, "Start mail input; end with <CRLF>.<CRLF>");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::TransactionFailed()
{
	return SMTPReply(554, "Transaction failed");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::MailboxSyntaxIncorrect()
{
	return SMTPReply(553, "Requested action not taken: mailbox syntax is incorrect");
}

bool ISXSMTP::SMTPReply::operator!=(const SMTPReply& other)
{
	PROFILE_FUNC();
	return !(*this == other);
}

bool ISXSMTP::SMTPReply::operator==(const SMTPReply& other)
{
	PROFILE_FUNC();
	return this->m_code == other.m_code;
}
