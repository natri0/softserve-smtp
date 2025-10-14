#pragma once

#include <cstdint>
#include <vector>

namespace ISXSMTP
{

/**
 * @brief Interface that declares how SMTPSession should interact with transmission channel (sockets)
 */
class ISMTPTransmissionChannel
{
public:
	/**
	 * @brief Writes given data to the transmission channel stream
	 * @param buffer with data
	 * @return number of written bytes
	 */
	virtual size_t Write(const std::vector<uint8_t>& buffer) = 0;

	/**
	 * @brief Read data into the given buffer
	 * @param buffer where to write data
	 * @return number of read bytes
	 */
	virtual size_t Read(std::vector<uint8_t>& buffer) = 0;

	/**
	 * @brief Returns true if there is data to read, false otherwise
	 * @return bool
	 */
	virtual bool IsDataAvailable() = 0;
};

}
