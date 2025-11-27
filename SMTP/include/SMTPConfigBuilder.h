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
	 * @param config SMTPConfig
	 */
	static void SetDefaultConfig(SMTPConfig config);

	/**
	 * @brief Sets domain for the config
	 * @param domain std::string
	 * @return *this for chaining calls
	 */
	SMTPConfigBuilder& SetDomain(std::string domain);

	/**
	 * @brief Sets context for the config
	 * @param context SMTPContext
	 * @return *this for chaining calls
	 */
	SMTPConfigBuilder& SetContext(SMTPContext context);

	/**
	 * @brief Sets mailbox for the config
	 * @param mailbox std::shared_ptr<SMTPIMailbox>
	 * @return *this for chaining calls
	 */
	SMTPConfigBuilder& SetMailbox(std::shared_ptr<SMTPIMailbox> mailbox);

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