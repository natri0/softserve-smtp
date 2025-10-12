#pragma once

#include "SMTPCommandBase.h"

#include <string>

namespace ISXSMTP
{

class QUITCommand : public SMTPCommandBase
{
public:
	QUITCommand();

	SMTPReply Invoke(SMTPCommandArguments arguments) override;
	SMTPString GetName() override;
	SMTPString GetSyntax() override;
};

}