#pragma once

#include "SMTPString.h"

#include <string>
#include <array>
#include <cstdint>

namespace ISXSMTP
{

class SMTPReply
{
public:
	SMTPReply(std::array<std::uint8_t, 3> code, const SMTPString& comment);

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
	static SMTPReply UserNotLocal251(); // will write user to forward path automatically
	static SMTPReply UserNotLocal551(); // will ask user to specifically set this user to forward path
	static SMTPReply CannotVerifyUser();
	static SMTPReply MailboxUnavailable450(); // mailbox may be available after some time
	static SMTPReply MailboxUnavailable550(); // mailbox unavailable permanently
	static SMTPReply ProcessingError();
	static SMTPReply InsufficientSystemStorage();
	static SMTPReply ExceededStorageAllocation();
	static SMTPReply StartMailInput();
	static SMTPReply TransactionFailed();
	
private:
	std::array<std::uint8_t, 3> m_code;
	SMTPString m_comment;	
};

}

