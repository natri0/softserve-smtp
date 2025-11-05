#include "SMTPClient.h"

ISXSMTP::SMTPClient::SMTPClient(
		const std::string& from,
		const std::string& to,
		std::shared_ptr<std::string> data,
		bool quit_after_data)
	: m_from(from)
	, m_to(to)
	, m_data(data)
	, m_quitAfterData(quit_after_data)
{
}

ISXSMTP::SMTPClient::SMTPClient()
	: m_from("")
	, m_to("")
	, m_data(nullptr)
	, m_quitAfterData(true)
{

}

void ISXSMTP::SMTPClient::SetTo(const std::string& to)
{
	m_to = to;
}

void ISXSMTP::SMTPClient::SetFrom(const std::string& from)
{
	m_from = from;
}

void ISXSMTP::SMTPClient::SetData(std::shared_ptr<std::string> data)
{
	m_data = data;
}

void ISXSMTP::SMTPClient::SetQuitAfterData(bool val)
{
	m_quitAfterData = val;
}

std::string ISXSMTP::SMTPClient::GetNextCommand()
{
	return "";	
}

std::string ISXSMTP::SMTPClient::Abort()
{
//	if (m_quitAfterData)
//		return GenQUITCommand();

//	return GenRSETCommand();

	return  "";
}

ISXSMTP::SMTPReplyResult OnReply(const std::string& reply)
{
	return ISXSMTP::SMTPReplyResult::OK;
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

bool ISXSMTP::SMTPClient::IsFinished()
{
	return m_state == ISXSMTP::SMTPStates::FINISH;
}

std::string ISXSMTP::SMTPClient::GenMAILCommand()
{
	return "";
}

std::string ISXSMTP::SMTPClient::GenRCPTCommand()
{
	return "";
}

std::string ISXSMTP::SMTPClient::GenDATACommand()
{
	return "";
}

std::string ISXSMTP::SMTPClient::GenEHLOCommand()
{
	return "";
}

std::string ISXSMTP::SMTPClient::GenQUITCommand()
{
	return "";
}

std::string ISXSMTP::SMTPClient::GenRSETCommand()
{
	return "";
}

