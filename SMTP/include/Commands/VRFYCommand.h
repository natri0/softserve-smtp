#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class VRFYCommand : public SMTPCommandBase
{
public:
	VRFYCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}