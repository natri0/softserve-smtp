#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class RSETCommand : public SMTPCommandBase
{
public:
	RSETCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}