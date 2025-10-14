#pragma once

#include "SMTPString.h"

#include <vector>
#include <cstdint>

namespace ISXSMTP
{

/**
 * @brief Class that represents a SMTP buffer to store data passed with SMTP commands
 */
class SMTPBuffer
{
private:
	SMTPString m_data;

public:
	/**
	 * @brief Appends data to the buffer
	 * @param data SMTPString
	 */
	void Append(const SMTPString& data);

	/**
	 * @brief Appends one character to the buffer
	 * @param data std::uint8_t 
	 */
	void Append(std::uint8_t data);

	/**
	 * @brief Returns reference to SMTPString that used to implement SMTPBuffer
	 * @return SMTPString&
	 */
	SMTPString& GetSMTPString();

	/**
	 * @brief Clears the data in the buffer
	 */
	void Clear();
};

}
