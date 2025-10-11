#pragma once

#include <cstdint>
#include <vector>

namespace ISXSMTP
{

class ITransmissionChannel
{
public:
	// Takes as the arguments pointer to the data and its count
	// Returns number of written bytes
	virtual size_t Write(const std::vector<uint8_t>& buffer) = 0;

	// Takes as the arguments buffer and its size (not in bytes but as the count)
	// Returns number of written bytes to the buffer
	virtual size_t Read(std::vector<uint8_t>& buffer) = 0;

	// Returns true if there's anything to read
	// and false otherwise
	virtual bool IsDataAvailable() = 0;
};

}
