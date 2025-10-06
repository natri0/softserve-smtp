#include "SMTPSession.h"

#include "Commands/DATACommand.h"
#include "Commands/RSETCommand.h"
#include "Commands/HELOCommand.h"
#include "Commands/EHLOCommand.h"
#include "Commands/HELPCommand.h"
#include "Commands/MAILCommand.h"
#include "Commands/RCPTCommand.h"
#include "Commands/NOOPCommand.h"
#include "Commands/VRFYCommand.h"
#include "Commands/QUITCommand.h"

#include <iostream>

ISXSMTP::SMTPSession::SMTPSession()
{
	fillCommandsVector();
	createCommandMap();
}

ISXSMTP::SMTPSession::~SMTPSession()
{
	// TODO
}

ISXSMTP::SMTPReply ISXSMTP::SMTPSession::ProcessClientCommand(std::string request)
{
	auto command_index = parseCommandVerb(request);

	auto reply = m_commands[command_index]->Invoke({});

	return reply;
}

std::string ISXSMTP::SMTPSession::GetOpeningMessage() const
{
	return std::string("220 <domain should be placed here> Simple Mail Transfer Service Ready");
}

std::string ISXSMTP::SMTPSession::GetClosingMessage() const
{
	return std::string("221 <domain should be placed here> Service closing transmission channel");
}

void ISXSMTP::SMTPSession::createCommandMap()
{
	for (size_t i = 0; i < m_commands.size(); i++)
	{
		m_commandMap[m_commands[i]->GetName()] = i;
	}
}

void ISXSMTP::SMTPSession::fillCommandsVector()
{
	m_commands.reserve(10);
	m_commands.emplace_back(std::make_unique<ISXSMTP::DATACommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::HELOCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::EHLOCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::HELPCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::MAILCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::RCPTCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::RSETCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::NOOPCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::VRFYCommand>());
	m_commands.emplace_back(std::make_unique<ISXSMTP::QUITCommand>());
}

size_t ISXSMTP::SMTPSession::parseCommandVerb(std::string_view request)
{
	// command verb ends with <SP>
	auto command_verb_end_pos = request.find_first_of(ISXSMTP::SP);
	if (command_verb_end_pos == std::string::npos)
	{
		// this could mean that command has no parameters
		// in this case string should end with <CRLF>
		command_verb_end_pos = request.find_first_of(ISXSMTP::CRLF);

		if (command_verb_end_pos == std::string::npos)
		{
			// error
			// TODO: handle error
			return {};
		}
	}

	// not using std::string_view because
	// there is no overloading in std::unordered_map::at()
	std::string command_verb = std::string(request.begin(), request.begin() + command_verb_end_pos);

	size_t command_index = 0;
	try
	{
		// get the command index in the vector
		command_index = m_commandMap.at(command_verb);
	}
	catch (...)
	{
		// TODO: handle error
	}

	return command_index;
}

