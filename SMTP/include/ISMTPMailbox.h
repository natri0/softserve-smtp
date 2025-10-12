#pragma once

#include "SMTPString.h"
#include "SMTPBuffer.h"

namespace ISXSMTP
{

class ISMTPMailbox
{
public:
	// arguments to this function might change in future
	virtual void DepositMail(SMTPBuffer forward_path, SMTPBuffer reverse_path, SMTPBuffer mail_data) = 0;

	virtual SMTPString GetName() = 0;
	
	virtual SMTPString GetLocalpart() = 0;
	
	// existence of next to methods is questionable
	// topic for future discuss
	virtual SMTPString GetDomain() = 0; 
	virtual SMTPString GetFullAddress() = 0;

	virtual bool IsMailboxAvailable(SMTPString localpart) = 0;
	virtual bool IsMailboxValid(SMTPString localpart) = 0;

	// this method should accept a part of an name and return 
	// full user name and user address
	virtual SMTPString SuggestAddress(SMTPString request) = 0;

	// this method should accept mailing list (one address that expands to multiple addresses)
	// and return all addresses that expands to this mailing list
	// existence of this method is a topic for future discuss
	// at this point I'm not sure if this functionality will (or should) be  implemented
	virtual SMTPString ExpandMailingList(SMTPString mail_list_address) = 0;
};

}