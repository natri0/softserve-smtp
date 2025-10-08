#pragma once

#include "SMTPReply.h"
#include "SMTPContext.h"

#include <string>
#include <memory>

namespace ISXSMTP
{

struct SMTPCommandArguments
{
	std::shared_ptr<SMTPContext> context;
};

class SMTPCommandBase 
{
public:
	virtual SMTPReply Invoke(SMTPCommandArguments arguments) = 0;
	virtual std::string GetName() const = 0;
};

}