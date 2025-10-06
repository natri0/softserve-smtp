#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class DATACommand : public SMTPCommandBase
{
public:
	DATACommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}