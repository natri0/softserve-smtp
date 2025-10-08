#pragma once

#include <cstdint>

namespace ISXSMTP
{

class ITransmissionChannel
{
public:
	// Takes as the arguments pointer to the data and its count
	// Returns number of written bytes
	virtual size_t Write(std::uint8_t* data, size_t count) = 0;

	// Takes as the arguments buffer and its size (not in bytes but as the count)
	// Returns number of written bytes to the buffer
	virtual size_t Read(std::uint8_t* buffer, size_t count) = 0;

	// Returns true if there's anything to read
	// and false otherwise
	virtual bool IsDataAvailable() = 0;
};

}
