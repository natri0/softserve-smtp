#include "SMTPBuffer.h"

void ISXSMTP::SMTPBuffer::Append(std::vector<std::uint8_t> data)
{
	m_data.append_range(data);
}

std::vector<std::uint8_t>& ISXSMTP::SMTPBuffer::Get()
{
	return m_data;
}

void ISXSMTP::SMTPBuffer::Clear()
{
	m_data.clear();
}

void ISXSMTP::SMTPBuffer::Append(std::uint8_t data)
{
	m_data.push_back(data);
}