#include "SMTPConfigBuilder.h"

#include <utility>

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::s_defaultConfig = {};

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::GetDefaultConfig()
{
	return s_defaultConfig;
}

void ISXSMTP::SMTPConfigBuilder::SetDefaultConfig(SMTPConfig config)
{
	s_defaultConfig = std::move(config);
}

void ISXSMTP::SMTPConfigBuilder::SetCurrentAsDefault()
{
	s_defaultConfig = m_config;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetDomain(std::string domain)
{
	m_config.domain = std::move(domain);
	return *this;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetContext(SMTPContext context)
{
	m_config.context = std::move(context);
	return *this;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetMailbox(std::shared_ptr<SMTPIMailbox> mailbox)
{
	m_config.mailbox = std::move(mailbox);
	return *this;
}

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::Build() const
{
	return m_config;
}
