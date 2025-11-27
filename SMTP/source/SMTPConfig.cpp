#include "SMTPConfig.h"

#include <utility>

ISXSMTP::SMTPConfig::SMTPConfig(
		std::shared_ptr<SMTPIMailbox> mailbox /*= std::make_shared<SMTPIMailbox>()*/, 
		std::string domain /*= "smtp.test"*/,
		SMTPContext context /*= {}*/)
	: domain(std::move(domain))
	, context(std::move(context))
	, mailbox(std::move(mailbox))
{
}
