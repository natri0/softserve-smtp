#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class HELOCommand : public SMTPCommandBase
{
public:
	HELOCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}