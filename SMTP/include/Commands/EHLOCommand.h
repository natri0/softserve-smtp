#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class EHLOCommand : public SMTPCommandBase
{
public:
	EHLOCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}