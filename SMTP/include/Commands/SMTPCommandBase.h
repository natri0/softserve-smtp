#pragma once

#include "SMTPReply.h"
#include "SMTPContext.h"
#include "SMTPCommandArguments.h"

#include <string>
#include <memory>

namespace ISXSMTP
{

/**
 * @brief Abstract class that defines interface of SMTPCommand
 */
class SMTPCommandBase 
{
public:
	/**
	 * @brief Executes command with provided arguments
	 * @param SMTPCommandArguments filled by SMTPCommandParser with attachment of context and mailbox
	 * @return Collection of and SMTPReply that might have one ore more replies
	 */
	virtual std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) = 0;

	/**
	 * @brief Returns command verb (first word of the command ended by <SP>)
	 * @return SMTPString
	 */
	virtual SMTPString GetName() = 0;

	/**
	 * @brief Returns command syntax that used in the SMTPCommandParser to parse the command
	 * @return SMTPString
	 */
	virtual SMTPString GetSyntax() = 0;
};

}
