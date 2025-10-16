#pragma once

#include "ISMTPMailbox.h"
#include "SMTPContext.h"
#include "Commands/SMTPCommandBase.h"

#include <memory>
#include <unordered_map>
#include <string>

namespace ISXSMTP
{

/**
 * @brief This class represents a SMTP session starting from the connection init to the QUIT command 
 */
class SMTPSession
{
private:
	std::shared_ptr<ISMTPMailbox> m_mailbox;
	std::shared_ptr<SMTPContext> m_context;
	std::unordered_map<std::string, std::unique_ptr<SMTPCommandBase>> m_commands;

	std::string m_clientInputBuffer;

public:
	/**
	 * @brief Constructor
	 * @param transmission_channel ISMTPTransmissionChannel
	 * @param mailbox ISMTPMailbox
	 */
	SMTPSession(std::shared_ptr<ISMTPMailbox> mailbox);

	/**
	 * @brief Returns true if the SMTPSession is finished its business with user
	 * @return bool
	 */
	bool IsFinished();

	/**
	 * @brief Should be called when client connects to the server
	 * @return std::string data for client
	 */
	std::string OnConnect();

	/**
	 * @brief Should be called when client sends data to the server
	 * @param message std::string data from client
	 * @return std::string data for client
	 */
	std::string OnMessage(const std::string& message);

private:
	/**
	 * @brief Fills std::unordered_map with commands and their command verbs (names)
	 */
	void fillCommandMap();

	/**
	 * @brief handles mail data input
	 * @param data std::string data from client
	 * @return bool returns true if user finished entering mail data
	 */
	bool handleMailDataInput(const std::string& data);

	bool IsDataEndingPresent(const std::string& data);
	bool IsLineEndingPresent(const std::string& data);
};

}
