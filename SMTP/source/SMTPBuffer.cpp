#include "SMTPBuffer.h"

ISXSMTP::SMTPBuffer::SMTPBuffer()
{

}

ISXSMTP::SMTPBuffer::~SMTPBuffer()
{

}

void ISXSMTP::SMTPBuffer::Clear()
{
	m_buffer.clear();
}

void ISXSMTP::SMTPBuffer::Write(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		m_buffer.push_back(str[i]);
	}
}

std::vector<std::uint8_t>& ISXSMTP::SMTPBuffer::Read()
{
	return m_buffer;
}

void ISXSMTP::SMTPBuffer::Write(std::uint8_t ch)
{
	m_buffer.push_back(ch);
}

void ISXSMTP::SMTPBuffer::Write(std::string_view str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		m_buffer.push_back(str[i]);
	}
}

