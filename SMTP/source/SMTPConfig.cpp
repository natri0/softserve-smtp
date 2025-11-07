#include "SMTPConfig.h"

ISXSMTP::SMTPConfig::SMTPConfig(
		std::shared_ptr<SMTPIMailbox> mailbox /*= std::make_shared<SMTPIMailbox>()*/, 
		const std::string& domain /*= "smtp.test"*/,
		const SMTPContext& context /*= {}*/)
	: domain(domain)
	, context(context)
	, mailbox(mailbox)
{
}
