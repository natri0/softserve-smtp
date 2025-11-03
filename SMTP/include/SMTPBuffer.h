#pragma once

#include <cstdint>
#include <string>

namespace ISXSMTP
{

/**
 * @brief Class that represents a SMTP buffer to store data passed with SMTP commands
 */
class SMTPBuffer
{
private:
	std::string m_data;

public:
	/**
	 * @brief Appends data to the buffer
	 * @param data std::string
	 */
	void Append(const std::string& data);

	/**
	 * @brief Appends one character to the buffer
	 * @param data std::uint8_t 
	 */
	void Append(std::uint8_t data);

	/**
	 * @brief Returns reference to std::string that used to store data in SMTPBuffer
	 * @return std::string&
	 */
	std::string& GetString();

	/**
	 * @brief Clears the data in the buffer
	 */
	void Clear();
};

}
