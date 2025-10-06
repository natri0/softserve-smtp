#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <string_view>

namespace ISXSMTP
{

class SMTPBuffer
{
public:
	SMTPBuffer();
	~SMTPBuffer();

	void Clear();
	void Write(std::string str);
	void Write(std::string_view str);
	void Write(std::uint8_t ch);
	std::vector<std::uint8_t>& Read();

private:
	std::vector<std::uint8_t> m_buffer;
};

}