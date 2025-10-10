#include "SMTPReply.h"

ISXSMTP::SMTPReply::SMTPReply(std::array<std::uint8_t, 3> code, const SMTPString& comment)
	: m_code(code)
	, m_comment(comment)
{
}

ISXSMTP::SMTPString ISXSMTP::SMTPReply::GetComment() const
{
	return m_comment;
}

std::array<std::uint8_t, 3> ISXSMTP::SMTPReply::GetCode() const
{
	return m_code;
}


ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CommandUnrecognized()
{
	return SMTPReply({ 5, 0, 0 }, "Command unrecognized");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::SyntaxError()
{
	return SMTPReply({ 5, 0, 1 }, "Syntax error in parameters or arguments");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CommandNotImplemented()
{
	return SMTPReply({ 5, 0, 2 }, "Command not implemented");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::BadSequenceOfCommands()
{
	return SMTPReply({ 5, 0, 3 }, "Bad sequence of commands");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CommandParameterNotImplemented()
{
	return SMTPReply({ 5, 0, 4 }, "Command parameter not implemented");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::HelpReply()
{
	return SMTPReply({ 2, 1, 1 }, "");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::HelpMessage()
{
	return SMTPReply({ 2, 1, 4 }, "");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ServiceReady()
{
	return SMTPReply({ 2, 2, 0 }, "<domain> Service ready");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ServiceClosing()
{
	return SMTPReply({ 2, 2, 1 }, "<domain> Service closing transmission channel");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ServiceNotAvailable()
{
	return SMTPReply({ 4, 2, 1 }, "Service not available, closing transmission channel");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::OK()
{
	return SMTPReply({ 2, 5, 0 }, "Action completed");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::UserNotLocal251()
{
	return SMTPReply({ 2, 5, 1 }, "User not local; will forward to <forward-path>");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::UserNotLocal551()
{
	return SMTPReply({ 5, 5, 1 }, "User not local; please try <forward-path>");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::CannotVerifyUser()
{
	return SMTPReply({ 2, 5, 2 }, "Cannot verify user, but will accept message and try to delivery");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::MailboxUnavailable450()
{
	return SMTPReply({ 4, 5, 0 }, "Mailbox unavailable, try later");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::MailboxUnavailable550()
{
	return SMTPReply({ 5, 5, 0 }, "Mailbox unavailable");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ProcessingError()
{
	return SMTPReply({ 4, 5, 1 }, "Processing error, try later");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::InsufficientSystemStorage()
{
	return SMTPReply({ 4, 5, 2 }, "Insufficient system storage, try later");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::ExceededStorageAllocation()
{
	return SMTPReply({ 5, 5, 2 }, "Exceeded storage allocation");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::StartMailInput()
{
	return SMTPReply({ 5, 5, 3 }, "Mailbox syntax incorrect");
}

ISXSMTP::SMTPReply ISXSMTP::SMTPReply::TransactionFailed()
{
	return SMTPReply({ 5, 5, 4 }, "Transaction failed");
}
