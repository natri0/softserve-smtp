#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class DATACommand : public SMTPCommandBase
{
public:
	DATACommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}