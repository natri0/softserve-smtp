#include "SMTPConfig.h"

ISXSMTP::SMTPConfig::SMTPConfig(const std::string& domain /*= "smtp.test"*/, const SMTPContext& context /*= {}*/)
	: domain(domain)
	, context(context)
{
}
