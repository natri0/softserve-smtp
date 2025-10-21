#pragma once

#include <string>

#include "SMTPContext.h"

namespace ISXSMTP
{

class SMTPConfig
{
public:
	std::string domain;
	SMTPContext context;

	SMTPConfig(const std::string& domain = "smtp.test", const SMTPContext& context = {});
};

}