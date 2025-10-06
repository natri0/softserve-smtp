#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class RSETCommand : public SMTPCommandBase
{
public:
	RSETCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}