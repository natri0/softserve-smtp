#pragma once

#include <unordered_map>
#include <unordered_set>

namespace ISXSMTP
{

struct SMTPCommandArguments
{
	// key - arg name
	// value - arg value
	std::unordered_map<std::vector<std::uint8_t>, std::vector<std::uint8_t>> arguments;
};

}

