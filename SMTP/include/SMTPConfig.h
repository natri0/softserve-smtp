#pragma once

#include <string>
#include <memory>

#include "SMTPContext.h"
#include "SMTPIMailbox.h"

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

	SMTPConfig(std::shared_ptr<SMTPIMailbox> mailbox = nullptr, const std::string& domain = "smtp.test", const SMTPContext& context = {});
};

}