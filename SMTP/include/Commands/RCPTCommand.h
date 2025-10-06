#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class RCPTCommand : public SMTPCommandBase
{
public:
	RCPTCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}