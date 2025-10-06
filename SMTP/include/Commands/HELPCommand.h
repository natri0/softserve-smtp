#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class HELPCommand : public SMTPCommandBase
{
public:
	HELPCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}