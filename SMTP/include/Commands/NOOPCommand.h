#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class NOOPCommand : public SMTPCommandBase
{
public:
	std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}
