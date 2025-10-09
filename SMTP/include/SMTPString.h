#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <memory> // for std::hash

namespace ISXSMTP
{

class SMTPString
{
private:
	std::vector<std::uint8_t> m_data;

public:
	static constexpr size_t NPOS = -1;

public:
	SMTPString();
	SMTPString(const std::vector<std::uint8_t>& data);
	SMTPString(std::vector<std::uint8_t>&& data);
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

	void ToUpper();

	std::uint8_t operator[](size_t index) const;
	SMTPString& operator+(const SMTPString& other);
	SMTPString& operator+(std::uint8_t value);
	SMTPString& operator+(const std::vector<std::uint8_t>& data);

	friend struct std::hash<SMTPString>;
};

}

template<>
struct std::hash<ISXSMTP::SMTPString> {
	std::size_t operator()(ISXSMTP::SMTPString const& s) const noexcept {
		// Not the best design, probably should write my own hash function for std::vector
		std::string str;
		str.assign(s.m_data.begin(), s.m_data.end()); 
		return std::hash<std::string>{}(str);
	}
};