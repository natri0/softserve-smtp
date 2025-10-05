#pragma once

#include <string>

namespace ISXSMTP
{
struct SMTPCommandResult
{
	// existence of this struct is questionable
	// might be removed later

	// error code or 
	// other kind of result
	// should be placed here 
};

struct SMTPCommandArguments
{
	// existence of this class is questionable
	// might be removed later

	// arguments for the command should be placed here
};

class SMTPCommandBase 
{
public:
	virtual SMTPCommandResult Invoke(SMTPCommandArguments arguments) = 0;
	virtual std::string GetName() const = 0;
};

}