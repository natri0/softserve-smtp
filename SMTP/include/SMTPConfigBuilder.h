#pragma once

#include "SMTPConfig.h"

namespace ISXSMTP
{

class SMTPConfigBuilder
{
private:
	static SMTPConfig s_defaultConfig;

	SMTPConfig m_config;

public:
	static SMTPConfig GetDefaultConfig();
	static void SetDefaultConfig(const SMTPConfig& config);

	SMTPConfigBuilder& SetDomain(const std::string& domain);
	SMTPConfigBuilder& SetContext(const SMTPContext& context);
	SMTPConfig Build() const;
	void SetCurrentAsDefault();
};

}