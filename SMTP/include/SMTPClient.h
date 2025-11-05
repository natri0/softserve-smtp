#pragma once

#include "SMTPState.h"
#include "SMTPReply.h"

#include <vector>
#include <string>
#include <memory>
#include <optional>

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
	SMTPClient();

	SMTPClient(
		const std::string& from, 
		const std::string& to,
		std::shared_ptr<std::string> data,
		bool quit_after_data); 

	void SetTo(const std::string& to);
	void SetFrom(const std::string& from);
	void SetData(std::shared_ptr<std::string> data);
	void SetQuitAfterData(bool val);

	std::string GetNextCommand();
	std::string Abort();

	SMTPReplyResult OnReply(const std::string& reply);

	std::optional<SMTPReply> ParseReply(const std::string& reply);

	// returns true if client should close connection
	bool IsFinished();

private:
	inline std::string GenMAILCommand();
	inline std::string GenRCPTCommand();
	inline std::string GenDATACommand();
	inline std::string GenEHLOCommand();
	inline std::string GenQUITCommand();
	inline std::string GenRSETCommand();
};


}
