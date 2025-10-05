#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class RCPTCommand : public SMTPCommandBase
{
public:
	RCPTCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}