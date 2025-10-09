#pragma once

#include <unordered_map>
#include <unordered_set>

namespace ISXSMTP
{

struct SMTPCommandArguments
{
	// key - arg name
	// value - arg value
	std::unordered_map<SMTPString, SMTPString> arguments;
};

}

