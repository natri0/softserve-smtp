#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class QUITCommand : public SMTPCommandBase
{
public:
	QUITCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}