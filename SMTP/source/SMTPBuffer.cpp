#include "SMTPBuffer.h"

ISXSMTP::SMTPString& ISXSMTP::SMTPBuffer::GetSMTPString()
{
	return m_data;
}

void ISXSMTP::SMTPBuffer::Clear()
{
	m_data.Clear();
}

void ISXSMTP::SMTPBuffer::Append(std::uint8_t data)
{
	m_data.Append(data);
}

void ISXSMTP::SMTPBuffer::Append(const SMTPString& data)
{
	m_data.Append(data);
}
