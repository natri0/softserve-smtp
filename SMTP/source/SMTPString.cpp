#pragma once

#include "SMTPString.h"

#include <algorithm>

ISXSMTP::SMTPString::SMTPString()
	: m_data({})
{
}

ISXSMTP::SMTPString::SMTPString(const std::string& data)
{
	m_data.assign(data.begin(), data.end());
}

ISXSMTP::SMTPString::SMTPString(std::vector<std::uint8_t>&& data)
	: m_data(std::move(data))
{

}

ISXSMTP::SMTPString::SMTPString(const std::vector<std::uint8_t>& data)
	: m_data(data)
{
	
}

void ISXSMTP::SMTPString::Append(std::uint8_t ch)
{
	m_data.push_back(ch);
}

void ISXSMTP::SMTPString::Clear()
{
	m_data.clear();
}

void ISXSMTP::SMTPString::Append(const std::vector<std::uint8_t>& data)
{
	m_data.append_range(data);
}

void ISXSMTP::SMTPString::Append(const SMTPString& string)
{
	m_data.append_range(string.m_data);
}

std::vector<std::uint8_t>& ISXSMTP::SMTPString::GetData()
{
	return m_data;
}

std::uint8_t ISXSMTP::SMTPString::Get(size_t index) const
{
	return m_data[index];
}

ISXSMTP::SMTPString ISXSMTP::SMTPString::GetRange(size_t start, size_t end) const
{
	return SMTPString(std::vector<std::uint8_t>(m_data.begin() + start, m_data.begin() + end));	
}

size_t ISXSMTP::SMTPString::Count() const
{
	return m_data.size();
}

ISXSMTP::SMTPString& ISXSMTP::SMTPString::Concat(const SMTPString& other)
{
	m_data.append_range(other.m_data);
	return *this;
}

void ISXSMTP::SMTPString::ToUpper()
{
	// Unless specified otherwise encoding should be ASCII
	for (auto item : m_data)
	{
		item = std::toupper(item);
	}
}

std::string ISXSMTP::SMTPString::ToString() const
{
	return std::string(m_data.begin(), m_data.end());
}

std::uint8_t ISXSMTP::SMTPString::operator[](size_t index) const
{
	return Get(index);
}

ISXSMTP::SMTPString& ISXSMTP::SMTPString::operator+(const SMTPString& other)
{
	return Concat(other);
}

ISXSMTP::SMTPString& ISXSMTP::SMTPString::operator+(const std::vector<std::uint8_t>& data)
{
	Append(data);
	return *this;
}

ISXSMTP::SMTPString& ISXSMTP::SMTPString::operator+(std::uint8_t value)
{
	Append(value);
	return *this;
}

size_t ISXSMTP::SMTPString::FindFirstOf(const SMTPString& value) const
{
	bool sequence_holding = false;
	size_t first = NPOS;
	for (size_t i = 0, j = 0; i < this->Count() && j < value.Count(); i++)
	{
		if (this->Get(i) == value.Get(j))
		{
			if (j == 0)
				first = i;
			sequence_holding = true;
			j++;
		}
		else
		{
			sequence_holding = false;
			first = NPOS;
		}
	}
	return first;
}

size_t ISXSMTP::SMTPString::FindFirstOf(std::uint8_t value) const
{
	auto it = std::find(m_data.begin(), m_data.end(), value);
	if (it != m_data.end())
		return it - m_data.begin();
	return NPOS;
}
