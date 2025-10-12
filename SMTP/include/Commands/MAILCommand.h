#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class MAILCommand : public SMTPCommandBase
{
public:
	MAILCommand();

	std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}