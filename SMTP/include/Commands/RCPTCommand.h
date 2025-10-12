#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class RCPTCommand : public SMTPCommandBase
{
public:
	RCPTCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}