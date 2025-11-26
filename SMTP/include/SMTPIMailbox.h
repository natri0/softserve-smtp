#pragma once
#include "SMTPBuffer.h"

namespace ISXSMTP
{
/**
 * @brief Interface that declares how SMTPSession should interact with mailbox that should be stored in the database
 */
class SMTPIMailbox {
public:
	virtual ~SMTPIMailbox() = default;

	/**
	 * @brief This method should put the mail in to the database.
		The passed arguments and the return value is the topic for future discuss
		and might change in the future.
	 * @param forward_path buffer with the recipients addresses
	 * @param reverse_path buffer with the sender address
	 * @param mail_data message body
	 */
	virtual void DepositMail(SMTPBuffer forward_path, SMTPBuffer reverse_path, SMTPBuffer mail_data) = 0;

	/**
	 * @brief Checks if given mailbox is available
	 * @param localpart of a recipient address
	 * @return bool
	 */
	virtual bool IsMailboxAvailable(const std::string& localpart) = 0;

	/**
	 * @brief This method accepts a part of an name or localpart of an address
		and returns all users that matches
	 * @param request std::string with known part
	 * @return std::string with one or multiple users
	 */
	virtual std::string SuggestAddress(const std::string& request) = 0;
};

}