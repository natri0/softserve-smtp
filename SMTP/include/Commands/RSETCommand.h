#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class RSETCommand : public SMTPCommandBase
{
public:
	RSETCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}