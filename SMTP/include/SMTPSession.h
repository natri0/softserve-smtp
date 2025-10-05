#pragma once

#include "Commands/SMTPCommandsBase.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace ISXSMTP
{

constexpr char CRLF[3] = {0x0D, 0x0A, 0x00};
constexpr char SP = 0x20;

struct SMTPResult
{
	// error code or 
	// other kind of result
	// should be placed here later
};

class SMTPSession
{
public:
	SMTPSession();
	~SMTPSession();

	// !!! copies string !!!
	// might change this later
	SMTPResult ProcessClientCommand(std::string request); 

	// this functions may be removed later
	std::string GetOpeningMessage() const;
	std::string GetClosingMessage() const;
		 
private:
	// creates std::unordered_map<std::string, size_t> of available commands
	// where std::string is command name and size_t index in the m_commands vector
	void createCommandMap();

	// temporary function, will be removed later
	void fillCommandsVector(); 

private:
	// this vector should be filled either in constructor 
	// depending on some kind of config
	// or in some SMTPSessionBuilder class
	// haven't decided yet
	std::vector<std::unique_ptr<SMTPCommandBase>> m_commands;

	std::unordered_map<std::string, size_t> m_commandMap;

};

} // ISXSMTP