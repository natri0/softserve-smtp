#pragma once

#include "SMTPString.h"

#include <unordered_map>

namespace ISXSMTP
{

struct SMTPCommandArguments
{
	// key - arg name
	// value - arg value
	std::unordered_map<SMTPString, SMTPString> arguments;
};

}

