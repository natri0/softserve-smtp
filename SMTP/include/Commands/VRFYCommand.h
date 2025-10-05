#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class VRFYCommand : public SMTPCommandBase
{
public:
	VRFYCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}