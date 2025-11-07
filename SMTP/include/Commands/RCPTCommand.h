#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class RCPTCommand : public SMTPCommandBase
{
public:
	std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() override;
	std::string GetSyntax() override;
};

}
