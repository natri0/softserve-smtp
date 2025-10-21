#include "SMTPConfigBuilder.h"

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::s_defaultConfig = {};

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::GetDefaultConfig()
{
	return s_defaultConfig;
}

void ISXSMTP::SMTPConfigBuilder::SetDefaultConfig(const SMTPConfig& config)
{
	s_defaultConfig = config;
}

void ISXSMTP::SMTPConfigBuilder::SetCurrentAsDefault()
{
	s_defaultConfig = m_config;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetDomain(const std::string& domain)
{
	m_config.domain = domain;
	return *this;
}

ISXSMTP::SMTPConfigBuilder& ISXSMTP::SMTPConfigBuilder::SetContext(const SMTPContext& context)
{
	m_config.context = context;
	return *this;
}

ISXSMTP::SMTPConfig ISXSMTP::SMTPConfigBuilder::Build() const
{
	return m_config;
}
