#pragma once

#include "ISMTPTransmissionChannel.h"
#include "ISMTPMailbox.h"
#include "SMTPContext.h"
#include "Commands/SMTPCommandBase.h"

#include <memory>
#include <unordered_map>

namespace ISXSMTP
{

/**
 * @brief This class represents a SMTP session starting from the connection init to the QUIT command 
 */
class SMTPSession
{
private:
	std::shared_ptr<ISMTPTransmissionChannel> m_transmissionChannel;
	std::shared_ptr<ISMTPMailbox> m_mailbox;
	std::shared_ptr<SMTPContext> m_context;
	std::unordered_map<SMTPString, std::unique_ptr<SMTPCommandBase>> m_commands;

public:
	/**
	 * @brief Constructor
	 * @param transmission_channel ISMTPTransmissionChannel
	 * @param mailbox ISMTPMailbox
	 */
	SMTPSession(
			std::shared_ptr<ISMTPTransmissionChannel> transmission_channel,
			std::shared_ptr<ISMTPMailbox> mailbox);

	/**
	 * @brief Returns true if the SMTPSession is finished its business with user
	 * @return bool
	 */
	bool IsFinished();

private:
	/**
	 * @brief Method with loop that processes client command
	 */
	void process();

	/**
	 * @brief Fills std::unordered_map with commands and their command verbs (names)
	 */
	void fillCommandMap();
};

}
