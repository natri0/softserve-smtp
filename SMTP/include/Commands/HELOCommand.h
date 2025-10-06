#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class HELOCommand : public SMTPCommandBase
{
public:
	HELOCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}