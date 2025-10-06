#pragma once

#include "SMTPReply.h"

#include <string>

namespace ISXSMTP
{

struct SMTPCommandArguments
{
	// existence of this class is questionable
	// might be removed later

	// arguments for the command should be placed here
};

class SMTPCommandBase 
{
public:
	virtual SMTPReply Invoke(SMTPCommandArguments arguments) = 0;
	virtual std::string GetName() const = 0;
};

}