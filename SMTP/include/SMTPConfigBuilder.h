#pragma once

#include "SMTPConfig.h"

namespace ISXSMTP
{

/**
 * @brief This class builds config for SMTP
 */
class SMTPConfigBuilder
{
private:
	static SMTPConfig s_defaultConfig;

	SMTPConfig m_config;

public:
	/**
	 * @brief Returns default config
	 * @return SMTPConfig
	 */
	static SMTPConfig GetDefaultConfig();
	/**
	 * @brief Sets default config
	 * @param config const SMTPConfig&
	 */
	static void SetDefaultConfig(const SMTPConfig& config);

	/**
	 * @brief Sets domain for the config
	 * @param domain const std::string&
	 * @return *this for chaining calls
	 */
	SMTPConfigBuilder& SetDomain(const std::string& domain);

	/**
	 * @brief Sets context for the config
	 * @param context const SMTPContext&
	 * @return *this for chaining calls
	 */
	SMTPConfigBuilder& SetContext(const SMTPContext& context);

	/**
	 * @brief Builds config
	 * @return SMTPConfig
	 */
	SMTPConfig Build() const;

	/**
	 * @brief Sets current config as default
	 */
	void SetCurrentAsDefault();
};

}