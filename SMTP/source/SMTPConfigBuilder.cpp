#include "SMTPConfigBuilder.h"

#include "Profiler.h"

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::s_defaultConfig = {};

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::GetDefaultConfig()
{
	PROFILE_FUNC();

	return s_defaultConfig;
}

void ISXSMTP::SMTPConfigBuilder::SetDefaultConfig(const SMTPConfig& config)
{
	PROFILE_FUNC();

	s_defaultConfig = config;
}

void ISXSMTP::SMTPConfigBuilder::SetCurrentAsDefault()
{
	PROFILE_FUNC();

	s_defaultConfig = m_config;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetDomain(const std::string& domain)
{
	PROFILE_FUNC();

	m_config.domain = domain;
	return *this;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetContext(const SMTPContext& context)
{
	m_config.context = context;
	return *this;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetMailbox(std::shared_ptr<SMTPIMailbox> mailbox)
{
	PROFILE_FUNC();

	m_config.mailbox = mailbox;
	return *this;
}

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::Build() const
{
	PROFILE_FUNC();

	return m_config;
}
