#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class VRFYCommand : public SMTPCommandBase
{
public:
	VRFYCommand();

	std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}