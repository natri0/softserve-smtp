#pragma once

#include "SMTPString.h"

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
	SMTPString m_comment;
	bool m_multiLine;

public:
	/**
	 * @brief Constructor
	 * @param code std::uint16_t that represents reply code
	 * @param comment SMTPString 
	 * @param multi_line bool flag
	 */
	SMTPReply(std::uint16_t code, const SMTPString& comment, bool multi_line);

	/**
	 * @brief Constructor
	 * @param code std::uint16_t that represents reply code
	 * @param comment SMTPString
	 */
	SMTPReply(std::uint16_t code, const SMTPString& comment);

	/**
	 * @brief Returns comment associated with reply
	 * @return SMTPString
	 */
	SMTPString GetComment() const;

	/**
	 * @brief Returns code associated with reply
	 * @return std::uint16_t
	 */
	std::uint16_t GetCode() const;

	/**
	 * @brief Returns SMTPString that ready to be send back to client
	 * @return SMTPString
	 */
	SMTPString ToSMTPString() const;

	/**
	 * @brief Returns vector from the SMTPString returned in the SMTPReply::ToSMTPString method
	 * @return std::vector<std::uint8_t>
	 */
	std::vector<std::uint8_t> ToVector() const;

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
