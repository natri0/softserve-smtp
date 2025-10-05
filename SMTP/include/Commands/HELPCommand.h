#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class HELPCommand : public SMTPCommandBase
{
public:
	HELPCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}