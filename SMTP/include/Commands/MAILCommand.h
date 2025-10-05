#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class MAILCommand : public SMTPCommandBase
{
public:
	MAILCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}