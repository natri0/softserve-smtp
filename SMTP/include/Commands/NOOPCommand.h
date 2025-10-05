#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class NOOPCommand : public SMTPCommandBase
{
public:
	NOOPCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}