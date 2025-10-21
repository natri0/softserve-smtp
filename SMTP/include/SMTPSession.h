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
	std::string m_domain;

	static std::unordered_map<std::string, std::unique_ptr<SMTPCommandBase>> s_commands;

public:
	/**
	 * @brief Constructor
	 * @param optionally config can be passed, default used otherwise
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
	 * @brief Returns current context
	 * @return SMTPContext
	 */
	SMTPContext GetContext();

	/**
	 * @brief Applies passed config
	 * @param config const SMTPConfig& 
	 * @return true if successful, false otherwise
	 */
	bool ApplyConfig(const SMTPConfig& config);

	std::string GetDomain() const;

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

	/**
	 * @brief Checks if config is valid
	 * @param config const SMTPConfig&
	 * @return true if ok, false otherwise
	 */
	bool checkConfig(const SMTPConfig& config);

	/**
	 * @brief Checks is mail data has proper ending (<CLRF>.<CLRF>)
	 * @param data const std::string&
	 * @return true if proper ending is present, false otherwise
	 */
	bool IsDataEndingPresent(const std::string& data);

	
	/**
	 * @brief Checks is line has proper ending (<CLRF>)
	 * @param data const std::string&
	 * @return true if proper ending is present, false otherwise
	 */
	bool IsLineEndingPresent(const std::string& data);
};

}
