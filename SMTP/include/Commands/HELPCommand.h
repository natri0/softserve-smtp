#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class HELPCommand : public SMTPCommandBase
{
public:
	HELPCommand();

	std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}