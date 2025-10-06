#pragma once

#include "Commands/SMTPCommandsBase.h"
#include "SMTPReply.h"
#include "SMTPBuffer.h"
#include "SMTPState.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace ISXSMTP
{

// will be moved into separate file in the future
constexpr char CRLF[3] = {0x0D, 0x0A, 0x00};
constexpr char SP = 0x20;

class SMTPSession
{
public:
	SMTPSession();
	~SMTPSession();

	// !!! copies string !!!
	// might change this later
	SMTPReply ProcessClientCommand(std::string request); 
		 
private:
	// creates std::unordered_map<std::string, size_t> of available commands
	// where std::string is command name and size_t index in the m_commands vector
	void createCommandMap();

	// temporary function, will be removed later
	void fillCommandsVector(); 

	// returns index of command in the m_commands vector
	size_t parseCommandVerb(std::string_view request);

private:
	// this vector should be filled either in constructor 
	// depending on some kind of config
	// or in some SMTPSessionBuilder class
	// haven't decided yet
	std::vector<std::unique_ptr<SMTPCommandBase>> m_commands;

	std::unordered_map<std::string, size_t> m_commandMap;

	SMTPBuffer m_reversePath;
	SMTPBuffer m_forwardPath;
	SMTPBuffer m_mailData;

	SMTPState m_state;
};

} // ISXSMTP