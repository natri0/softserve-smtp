#include "SMTPClient.h"

#include <format>

ISXSMTP::SMTPClient::SMTPClient(
	const std::string& domain,
	const std::string& from,
	const std::vector<std::string>& to,
	bool quit_after_data)
	: m_from(from)
	, m_to(to)
	, m_quitOnFinish(quit_after_data)
	, m_domain(domain)
{
}

ISXSMTP::SMTPClient::SMTPClient()
	: m_from("")
	, m_to({})
	, m_domain("smtp.test")
	, m_quitOnFinish(true)
{
}

void ISXSMTP::SMTPClient::SetTo(const std::vector<std::string>& to)
{
	m_to = to;
}

void ISXSMTP::SMTPClient::SetFrom(const std::string& from)
{
	m_from = from;
}

void ISXSMTP::SMTPClient::SetDomain(const std::string& domain)
{
	m_domain = domain;
}

void ISXSMTP::SMTPClient::SetQuitOnFinish(bool val)
{
	m_quitOnFinish = val;
}

std::vector<std::string> ISXSMTP::SMTPClient::GenCommands()
{
	std::vector<std::string> commands;
	commands.push_back(GenEHLOCommand());
	commands.push_back(GenMAILCommand());
	for (const auto& i : m_to)
		commands.push_back(GenRCPTCommand(i));
	commands.push_back(GenDATACommand());

	if (m_quitOnFinish)
		commands.push_back(GenQUITCommand());
	else
		commands.push_back(GenRSETCommand());

	return commands;
}

ISXSMTP::SMTPTransactionStatus ISXSMTP::SMTPClient::OnReply(const std::string& reply)
{
	auto parsed_reply = ParseReply(reply);
	if (!parsed_reply.has_value())
		return SMTPTransactionStatus::REPLY_PARSE_ERROR;
	if (parsed_reply->IsMultiLine())
		return SMTPTransactionStatus::WAIT_FOR_REPLY;
	if (parsed_reply->GetCode() >= 500)
		return SMTPTransactionStatus::ABORTED;
	if (parsed_reply->GetCode() >= 400)
		return SMTPTransactionStatus::RETRY_LATER;
	if (parsed_reply->GetCode() >= 300)
		return SMTPTransactionStatus::SEND_DATA;

	return SMTPTransactionStatus::SEND_NEXT_COMMAND;
}

std::string ISXSMTP::SMTPClient::OnAbort()
{
	if (m_quitOnFinish)
		return GenQUITCommand();
	return GenRSETCommand();
}

std::optional<ISXSMTP::SMTPReply> ISXSMTP::SMTPClient::ParseReply(const std::string& reply)
{
	if (reply.size() < 3)
		return {};

	size_t chars_read = 0;
	uint16_t code = 0;
	try
	{
		code = std::stoi(reply, &chars_read, 10);
	}
	catch (...)
	{
		return {};
	}

	if (chars_read != 3)
		return {};

	if (chars_read >= reply.size())
		return SMTPReply(code, "", false);

	bool multi_line = false;
	if (reply[chars_read] == '-')
		multi_line = true;
	else if (reply[chars_read] != ' ')
		return {};

	std::string comment = std::string(reply.begin() + chars_read + 1, reply.end());

	return SMTPReply(code, comment, multi_line);
}

std::string ISXSMTP::SMTPClient::GenMAILCommand()
{
	return std::format("MAIL FROM:<{}>\r\n", m_from);
}

std::string ISXSMTP::SMTPClient::GenRCPTCommand(const std::string& to)
{
	return std::format("RCPT TO:<{}>\r\n", to);
}

std::string ISXSMTP::SMTPClient::GenDATACommand()
{
	return "DATA\r\n";
}

std::string ISXSMTP::SMTPClient::GenEHLOCommand()
{
	return "EHLO " + m_domain + "\r\n";
}

std::string ISXSMTP::SMTPClient::GenQUITCommand()
{
	return "QUIT\r\n";
}

std::string ISXSMTP::SMTPClient::GenRSETCommand()
{
	return "RSET\r\n";
}
