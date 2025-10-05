#pragma once

#include "SMTPCommandsBase.h"

#include <string>

namespace ISXSMTP
{

class QUITCommand : public SMTPCommandBase
{
public:
	QUITCommand();

	SMTPCommandResult Invoke(SMTPCommandArguments arguments) override;
	std::string GetName() const override;
};

}