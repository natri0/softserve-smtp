#include "SMTPBuffer.h"

#include "Profiler.h"

std::string& ISXSMTP::SMTPBuffer::GetString()
{
	PROFILE_FUNC();

	return m_data;
}

void ISXSMTP::SMTPBuffer::Clear()
{
	PROFILE_FUNC();

	m_data.clear();
}

void ISXSMTP::SMTPBuffer::Append(std::uint8_t data)
{
	PROFILE_FUNC();

	m_data.append(1, data);
}

void ISXSMTP::SMTPBuffer::Append(const std::string& data)
{
	PROFILE_FUNC();

	m_data.append(data);
}
