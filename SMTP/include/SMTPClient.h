#pragma once

#include "SMTPState.h"

#include <vector>
#include <string>
#include <memory>

namespace ISXSMTP
{

enum struct SMTPReplyResult // naming is hard
{
	ABORT, // SMTPClient::Abort() should be called to get next command for the server 
	RETRY_LATER, 
	OK // call SMTPClient::GetNextCommand()
};

class SMTPClient
{
private:
	std::string m_from; 
	// TODO: add support for multiple rcpt
	std::string m_to;
	std::shared_ptr<std::string> m_data;
	SMTPState m_state;
	bool m_quitAfterData;

public:
	SMTPClient(
		const std::string& from, 
		const std::string& to,
		std::shared_ptr<std::string> data,
		bool quit_after_data); 

	std::string GetNextCommand();
	std::string Abort();

	SMTPReplyResult OnReply(const std::string& reply);

	// retuns true if client should close connection
	bool IsFinished();

private:
	std::string GenMAILCommand();
	std::string GenRCPTCommand();
	std::string GenDATACommand();
	std::string GenEHLOCommand();
	std::string GenQUITCommand();
	std::string GenRSETCommand();
};


}
