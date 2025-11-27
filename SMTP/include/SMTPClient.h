#pragma once

#include "SMTPState.h"
#include "SMTPReply.h"

#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace ISXSMTP
{

enum struct SMTPTransactionStatus 
{
	SEND_NEXT_COMMAND,
	ABORTED,
	RETRY_LATER,
	REPLY_PARSE_ERROR,
	WAIT_FOR_REPLY,
	SEND_DATA
};

class SMTPClient
{
private:
	std::string m_from; 
	std::vector<std::string> m_to;
	std::string m_domain;
	bool m_quitOnFinish;

public:
	SMTPClient();

	SMTPClient(
		std::string domain,
		std::string from,
		std::vector<std::string> to,
		bool quit_on_finish = true);

	void SetTo(std::vector<std::string> to);
	void SetFrom(std::string from);
	void SetDomain(std::string domain);
	void SetQuitOnFinish(bool val);

	std::vector<std::string> GenCommands();
	SMTPTransactionStatus OnReply(const std::string& reply);
	std::string OnAbort();

	std::optional<SMTPReply> ParseReply(const std::string& reply);

private:
	inline std::string GenMAILCommand();
	inline std::string GenRCPTCommand(const std::string& to);
	inline std::string GenDATACommand();
	inline std::string GenEHLOCommand();
	inline std::string GenQUITCommand();
	inline std::string GenRSETCommand();
	
};

}
