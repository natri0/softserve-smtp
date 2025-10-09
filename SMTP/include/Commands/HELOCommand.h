#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class HELOCommand : public SMTPCommandBase
{
public:
	HELOCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}