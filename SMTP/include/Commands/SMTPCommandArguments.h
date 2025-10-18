#pragma once

#include "ISMTPMailbox.h"
#include "SMTPContext.h"

#include <unordered_map>
#include <string>
#include <memory>

namespace ISXSMTP
{

/**
 * @brief Struct that contains arguments parsed by SMTPCommandParser 
	and shared pointers to context and mailbox from SMTPSession
 */
struct SMTPCommandArguments
{
	SMTPCommandArguments(
			SMTPContext& context,
			std::unordered_map<std::string, std::string> arguments,
			std::shared_ptr<ISMTPMailbox> mailbox)
		: context(context)
		, arguments(arguments)
		, mailbox(mailbox)
	{
	}

	std::unordered_map<std::string, std::string> arguments;
	SMTPContext& context;
	std::shared_ptr<ISMTPMailbox> mailbox;
};

}
