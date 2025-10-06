#pragma once

#include "SMTPReply.h"
#include "SMTPBuffer.h"
#include "SMTPState.h"

#include <string>

namespace ISXSMTP
{

struct SMTPCommandArguments
{
	// existence of this class is questionable
	// might be removed later

	// arguments for the command should be placed here

	SMTPBuffer& reverse_path;
	SMTPBuffer& forward_path;
	SMTPBuffer& mail_data;

	SMTPState& state;

	std::string request;
};

class SMTPCommandBase 
{
public:
	virtual SMTPReply Invoke(SMTPCommandArguments arguments) = 0;
	virtual std::string GetName() const = 0;
};

}