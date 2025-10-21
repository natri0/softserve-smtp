#pragma once

#include <string>

#include "SMTPContext.h"

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

	SMTPConfig(const std::string& domain = "smtp.test", const SMTPContext& context = {});
};

}