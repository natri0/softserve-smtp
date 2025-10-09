#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace ISXSMTP
{

class SMTPString
{
private:
	std::vector<std::uint8_t> m_data;

public:
	SMTPString();
	SMTPString(const std::vector<std::uint8_t>& data);
	SMTPString(std::vector<std::uint8_t> data);
	SMTPString(std::vector<std::uint8_t>&& data);
	SMTPString(std::string data);
	SMTPString(const std::string& data);

	void Append(std::uint8_t ch);
	void Append(const SMTPString& string);
	void Append(const std::vector<std::uint8_t>& data);

	void Clear();

	std::vector<std::uint8_t>& GetData();
	std::uint8_t Get(size_t index) const;
	size_t Count() const;

	size_t FindFirstOf(std::uint8_t value) const;
	size_t FindFirstOf(const SMTPString& value) const;

	SMTPString& Concat(const SMTPString& other);

	std::uint8_t operator[](size_t index) const;
	SMTPString& operator+(const SMTPString& other);
	SMTPString& operator+(std::uint8_t value);
	SMTPString& operator+(const std::vector<std::uint8_t>& data);
};

}