#include "SMTPBuffer.h"

std::string& ISXSMTP::SMTPBuffer::GetString()
{
	return m_data;
}

void ISXSMTP::SMTPBuffer::Clear()
{
	m_data.clear();
}

void ISXSMTP::SMTPBuffer::Append(std::uint8_t data)
{
	m_data.append(1, data);
}

void ISXSMTP::SMTPBuffer::Append(const std::string& data)
{
	m_data.append(data);
}
