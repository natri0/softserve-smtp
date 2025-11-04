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

bool ISXSMTP::SMTPClient::IsFinished()
{
	return m_state == ISXSMTP::SMTPStates::FINISH;
}


