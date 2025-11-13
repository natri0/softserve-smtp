#pragma once

#include "SMTPContext.h"
#include "SMTPIMailbox.h"
#include "IAuthHandler.h"

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
				const std::string& domain,
				std::shared_ptr<SMTPIMailbox> mailbox,
				std::shared_ptr<IAuthHandler> auth_handler)
			: context(context)
			, arguments(arguments)
			, domain(domain)
			, mailbox(mailbox)
			, auth_handler(auth_handler)
		{
		}

		std::unordered_map<std::string, std::string> arguments;
		SMTPContext& context;
		std::string domain;
		std::shared_ptr<SMTPIMailbox> mailbox;
		std::shared_ptr<IAuthHandler> auth_handler;
	};

}
