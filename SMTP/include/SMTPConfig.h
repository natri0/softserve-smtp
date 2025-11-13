#pragma once

#include <string>
#include <memory>

#include "SMTPContext.h"
#include "SMTPIMailbox.h"
#include "IAuthHandler.h"

namespace ISXSMTP
{

/**
 * @brief This struct represents config for SMTP
 */
struct SMTPConfig
{
public:
	std::string domain;
	SMTPContext context;
	std::shared_ptr<SMTPIMailbox> mailbox;
	std::shared_ptr<IAuthHandler> auth_handler;

	SMTPConfig(std::shared_ptr<SMTPIMailbox> mailbox = nullptr, std::shared_ptr<IAuthHandler> auth_handler = nullptr, const std::string& domain = "smtp.test", const SMTPContext& context = {});
};

}