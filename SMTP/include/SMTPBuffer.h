#pragma once

#include "SMTPString.h"

#include <vector>
#include <cstdint>

namespace ISXSMTP
{

class SMTPBuffer
{
private:
	SMTPString m_data;

public:
	void Append(const SMTPString& data);
	void Append(std::uint8_t data);

	SMTPString& Get();

	void Clear();
};

}