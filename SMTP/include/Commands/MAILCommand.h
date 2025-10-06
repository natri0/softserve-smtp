#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class MAILCommand : public SMTPCommandBase
{
public:
	MAILCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}