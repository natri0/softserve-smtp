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
	virtual SMTPReply Invoke(SMTPCommandArguments arguments, std::shared_ptr<SMTPContext> context) = 0;
	virtual std::vector<std::uint8_t> GetName() = 0;
	virtual std::vector<std::uint8_t> GetSyntax() = 0;
};

}