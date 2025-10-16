#pragma once

#include <string>
#include <array>
#include <cstdint>

namespace ISXSMTP
{

/**
 * @brief Class that represents reply that SMTP send to the client
 */
class SMTPReply
{
private:
	std::uint16_t m_code;
	std::string m_comment;
	bool m_multiLine;

public:
	/**
	 * @brief Constructor
	 * @param code std::uint16_t that represents reply code
	 * @param comment std::string 
	 * @param multi_line bool flag
	 */
	SMTPReply(std::uint16_t code, const std::string& comment, bool multi_line);

	/**
	 * @brief Constructor
	 * @param code std::uint16_t that represents reply code
	 * @param comment std::string
	 */
	SMTPReply(std::uint16_t code, const std::string& comment);

	/**
	 * @brief Returns comment associated with reply
	 * @return std::string
	 */
	std::string GetComment() const;

	/**
	 * @brief Returns code associated with reply
	 * @return std::uint16_t
	 */
	std::uint16_t GetCode() const;

	/**
	 * @brief Returns std::string that ready to be send back to client
	 * @return std::string
	 */
	std::string ToString() const;

	/**
	 * @brief Sets multi line flag
	 * @param value bool
	 */
	void SetMultiLine(bool value);

public:
	static SMTPReply CommandUnrecognized();
	static SMTPReply SyntaxError();
	static SMTPReply CommandNotImplemented();
	static SMTPReply BadSequenceOfCommands();
	static SMTPReply CommandParameterNotImplemented();
	static SMTPReply HelpReply();
	static SMTPReply HelpMessage();
	static SMTPReply ServiceReady();
	static SMTPReply ServiceClosing();
	static SMTPReply ServiceNotAvailable();
	static SMTPReply OK();
	static SMTPReply UserNotLocal251(); 
	static SMTPReply UserNotLocal551(); 
	static SMTPReply CannotVerifyUser();
	static SMTPReply MailboxUnavailable450();
	static SMTPReply MailboxUnavailable550(); 
	static SMTPReply ProcessingError();
	static SMTPReply InsufficientSystemStorage();
	static SMTPReply ExceededStorageAllocation();
	static SMTPReply StartMailInput();
	static SMTPReply TransactionFailed();
	
	bool operator==(const SMTPReply& other);
	bool operator!=(const SMTPReply& other);
};

}
