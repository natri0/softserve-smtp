#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <memory> // for std::hash

namespace ISXSMTP
{

/**
 * @brief That class represents strings in SMTP
 *	Decision to write separate string class for SMTP
 *  came from the fact that SMTP need a guarantee that all characters are 8 bit
 *  and std::string with unsigned char underneath guaranties that all character
 *  are AT LEAST 8 bits not exactly 8 bits
 */
class SMTPString
{
private:
	std::vector<std::uint8_t> m_data;

public:
	static constexpr size_t NPOS = -1;

public:
	/**
	 * @brief Default constructor
	 */
	SMTPString();

	/**
	 * @brief Constructing SMTPString from vector
	 * @param data const std::vector<std::uint8_t>&
	 */
	SMTPString(const std::vector<std::uint8_t>& data);

	/**
	 * @brief Constructing SMTPString from rvalue vector
	 * @param data std::vector<std::uint8_t>&&
	 */
	SMTPString(std::vector<std::uint8_t>&& data);

	/**
	 * @brief Constructing SMTPString from std::string
	 * Might not work on system where unsigned char is not 8 bits 
	 * @param data const std::string& data
	 */
	SMTPString(const std::string& data);

	/**
	 * @brief Constructing SMTPString from c string
	 * * Might not work on system where char is not 8 bits 
	 * @param cstring const char*
	 */
	SMTPString(const char* cstring);

	/**
	 * @brief Appends one character to the string
	 * @param ch std::uint8_t
	 */
	void Append(std::uint8_t ch);

	/**
	 * @brief Appends another SMTPString to the string
	 * @param string SMTPString
	 */
	void Append(const SMTPString& string);

	/**
	 * @brief Appends vector to the string
	 * @param data std::vector<std::uint8_t>
	 */
	void Append(const std::vector<std::uint8_t>& data);

	/**
	 * @brief Clears data in the string
	 */
	void Clear();

	/**
	 * @brief returns true if string is empty, false otherwise
	 * @return bool
	 */
	bool IsEmpty() const;

	/**
	 * @brief Returns reference to the vector that used underneath to implement SMTPString
	 * @return 
	 */
	std::vector<std::uint8_t>& GetData();

	/**
	 * @brief Returns single character from the string by index
	 * @param index size_t
	 * @return std::uint8_t
	 */
	std::uint8_t Get(size_t index) const;

	/**
	 * @brief Returns range of character from string
	 * @param start size_t
	 * @param end size_t
	 * @return SMTPString
	 */
	SMTPString GetRange(size_t start, size_t end) const;

	/**
	 * @brief Returns how many characters string contains
	 * @return size_t
	 */
	size_t Count() const;

	/**
	 * @brief Returns index of first occurrence of character
	 * @param value std::uint8_t value
	 * @return size_t index 
	 */
	size_t FindFirstOf(std::uint8_t value) const;

	/**
	 * @brief Returns index of first occurrence of sequence of characters
	 * @param value std::uint8_t value
	 * @return size_t index
	 */
	size_t FindFirstOf(const SMTPString& value) const;

	/**
	 * @brief Concatenates two string
	 * @param other string
	 * @return reference to this string
	 */
	SMTPString& Concat(const SMTPString& other);

	/**
	 * @brief Checks if line terminated by <CRLF>
	 * @return bool
	 */
	bool IsEndingPresent() const;

	/**
	 * @brief Checks if line terminated by <CRLF>.<CRLF>
	 * @return bool
	 */
	bool IsDataEndingPresent() const;

	/**
	 * @brief Makes all characters in the string upper case
	 */
	void ToUpper();

	/**
	 * @brief Convers SMTPString to std::string
	 * @return 
	 */
	std::string ToString() const;

	/**
	 * @brief Operator overload for SMTPString::Get method
	 * @param index size_t
	 * @return std::uint8_t
	 */
	std::uint8_t operator[](size_t index) const;

	/**
	 * @brief Operator overload for SMTPString::Concat method
	 * @param other SMTPString
	 * @return reference to this string
	 */
	SMTPString& operator+(const SMTPString& other);

	/**
	 * @brief Operator overload for SMTPString::Append method
	 * @param other std::uint8_t value
	 * @return reference to this string
	 */
	SMTPString& operator+(std::uint8_t value);

	/**
	 * @brief Operator overload for SMTPString::Append method
	 * @param data std::vector<std::uint8_t> 
	 * @return reference to this string
	 */
	SMTPString& operator+(const std::vector<std::uint8_t>& data);

	/**
	 * @brief Operator overload for compressing two strings
	 * @param other SMTPString
	 * @return bool
	 */
	bool operator==(const SMTPString& other) const;

	/**
	 * @brief Friend struct for hashing
	 */
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
