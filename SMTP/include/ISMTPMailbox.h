#pragma once

#include "SMTPString.h"
#include "SMTPBuffer.h"

namespace ISXSMTP
{

/**
 * @brief Interface that declares how SMTPSession should interact with mailbox that should be stored in the database
 */
class ISMTPMailbox
{
public:
	/**
	 * @brief This method should put the mail in to the database. 
		The passed arguments and the return value is the topic for future discuss 
		and might change in the future.
	 * @param forward_path buffer with the sender address
	 * @param reverse_path buffer with the recipient address
	 * @param mail_data message body 
	 */
	virtual void DepositMail(SMTPBuffer forward_path, SMTPBuffer reverse_path, SMTPBuffer mail_data) = 0;

	/**
	 * @brief Checks if given mailbox is available
	 * @param localpart of a recipient address
	 * @return bool
	 */
	virtual bool IsMailboxAvailable(SMTPString localpart) = 0;

	// this method should accept a part of an name and return 
	// full user name and user address
	
	/**
	 * @brief This method accepts a part of an name or localpart of an address 
		and returns all users that matches 
	 * @param request SMTPString with known part
	 * @return SMTPString with one or multiple users
	 */
	virtual SMTPString SuggestAddress(SMTPString request) = 0;

	/**
	 * @brief This method accepts address that represents mailing list 
	 *	and returns all addresses that corresponds to that mailing list
	 * @param mail_list_address address of a mailing list
	 * @return SMTPString with addresses
	 */
	virtual SMTPString ExpandMailingList(SMTPString mail_list_address) = 0;
};

}
