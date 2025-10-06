#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class EHLOCommand : public SMTPCommandBase
{
public:
	EHLOCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}