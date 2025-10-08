#pragma once

#include <vector>
#include <cstdint>

namespace ISXSMTP
{

class SMTPBuffer
{
private:
	std::vector<std::uint8_t> m_data;

public:
	void Append(std::vector<std::uint8_t> data);
	void Append(std::uint8_t data);

	std::vector<std::uint8_t>& Get();

	void Clear();
};

}