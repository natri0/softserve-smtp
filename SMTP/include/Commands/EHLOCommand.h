#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class EHLOCommand : public SMTPCommandBase
{
public:
	EHLOCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}