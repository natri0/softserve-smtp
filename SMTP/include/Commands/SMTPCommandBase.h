#pragma once

#include "SMTPReply.h"
#include "SMTPContext.h"
#include "SMTPCommandArguments.h"

#include <string>
#include <vector>
#include <memory>
#include <vector>

namespace ISXSMTP
{

/**
 * @brief Abstract class that defines interface of SMTPCommand
 */
class SMTPCommandBase 
{
public:
	virtual ~SMTPCommandBase() = default;

	/**
	 * @brief Executes command with provided arguments
	 * @param SMTPCommandArguments filled by SMTPCommandParser with attachment of context and mailbox
	 * @return Collection of and SMTPReply that might have one ore more replies
	 */
	virtual std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) = 0;

	/**
	 * @brief Returns command verb (first word of the command ended by <SP>)
	 * @return std::string
	 */
	virtual std::string GetName() = 0;

	/**
	 * @brief Returns command syntax that used in the SMTPCommandParser to parse the command
	 * @return std::string
	 */
	virtual std::string GetSyntax() = 0;
};

}
