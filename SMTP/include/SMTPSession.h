#pragma once

#include "SMTPContext.h"
#include "Commands/SMTPCommandBase.h"
#include "SMTPConfig.h"
#include "SMTPConfigBuilder.h"

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
	SMTPContext m_context;
	std::string m_clientInputBuffer;

	static std::unordered_map<std::string, std::unique_ptr<SMTPCommandBase>> s_commands;

public:
	/**
	 * @brief Constructor
	 * @param optionally context SMTPContext can be passed
	 */
	SMTPSession(const SMTPConfig& config = ISXSMTP::SMTPConfigBuilder::GetDefaultConfig());

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

	/**
	 * @brief Returns pointer to current context
	 * @return SMTPContext
	 */
	SMTPContext GetContext();

	bool ApplyConfig(const SMTPConfig& config);

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

	bool checkConfig(const SMTPConfig& config);

	bool IsDataEndingPresent(const std::string& data);
	bool IsLineEndingPresent(const std::string& data);
};

}
