#pragma once

#include <string>

namespace ISXSMTP
{

struct SMTPReply
{
	// error code or 
	// other kind of result
	// should be placed here later

	// SMTP returns 3 digit code (+1 for \0) depending on the result of operation
	char reply_code[4];
	std::string command;
};

}

