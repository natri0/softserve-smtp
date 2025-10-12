#pragma once

#include "SMTPReply.h"
#include "SMTPContext.h"
#include "SMTPCommandArguments.h"

#include <string>
#include <memory>

namespace ISXSMTP
{

class SMTPCommandBase 
{
public:
	virtual SMTPReply Invoke(SMTPCommandArguments arguments) = 0;
	virtual SMTPString GetName() = 0;
	virtual SMTPString GetSyntax() = 0;
};

}