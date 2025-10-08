#pragma once

#include "Commands/SMTPCommandsBase.h"

#include <vector>

namespace ISXSMTP
{

class CommandParser
{
public:
	// static class
	CommandParser() = delete; 

	static SMTPCommandArguments Parse(const std::vector<uint8_t>& command);
};

}