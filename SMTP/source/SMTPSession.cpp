#include "SMTPSession.h"
#include "Commands/EHLOCommand.h"
#include "Commands/MAILCommand.h"
#include "Commands/RCPTCommand.h"
#include "Commands/DATACommand.h"
#include "Commands/HELPCommand.h"
#include "Commands/NOOPCommand.h"
#include "Commands/QUITCommand.h"
#include "Commands/RSETCommand.h"
#include "Commands/HELOCommand.h"
#include "SMTPConstants.h"
#include "SMTPCommandParser.h"

#include <iostream>

ISXSMTP::SMTPSession::SMTPSession(std::shared_ptr<ISMTPMailbox> mailbox)
	: m_mailbox(mailbox)
{
	fillCommandMap();
	m_context = std::make_shared<SMTPContext>();

	//process();
}

bool ISXSMTP::SMTPSession::IsFinished()
{
	if (m_context->state == ISXSMTP::SMTPStates::FINISH)
		return true;
	return false;
}

std::vector<std::uint8_t> ISXSMTP::SMTPSession::OnConnect()
{
	return SMTPReply::ServiceReady().ToVector();
}

std::vector<std::uint8_t> ISXSMTP::SMTPSession::OnMessage(std::vector<std::uint8_t> message)
{
	if (m_context->state == SMTPStates::POST_DATA)
	{
		// handle mail data
		if (handleMailDataInput(message))
		{
			return SMTPReply::OK().ToVector();
		}

		// client hasn't finished inputing mail data
		return {};
	}

	m_clientInputBuffer.Append(message);
	if (!m_clientInputBuffer.IsEndingPresent())
	{
		// this means client didn't finish sending command
		return {};
	}

	auto command_parser_result = SMTPCommandParser::Parse(m_clientInputBuffer, m_commands);
	if (command_parser_result.error_code != SMTPReply::OK())
	{
		// failed to parse command
		return command_parser_result.error_code.ToVector();
	}

	// bind context and mailbox
	command_parser_result.parsed_arguments.context = m_context;
	command_parser_result.parsed_arguments.mailbox = m_mailbox;
	auto command_result = m_commands[command_parser_result.command_verb]->Invoke(command_parser_result.parsed_arguments);
	
	m_clientInputBuffer.Clear();

	SMTPString reply;
	for (auto i : command_result)
	{
		reply.Append(i.ToSMTPString());
	}
	return reply.GetData();
}

bool ISXSMTP::SMTPSession::handleMailDataInput(std::vector<std::uint8_t> data)
{
	m_context->mail_data.Append(data);
	if (m_context->mail_data.GetSMTPString().IsDataEndingPresent())
	{
		m_context->state = SMTPStates::END_DATA;
		return true;
	}
	return false;
}

void ISXSMTP::SMTPSession::fillCommandMap()
{
	auto EHLO = std::make_unique<EHLOCommand>();
	auto MAIL = std::make_unique<MAILCommand>();
	auto RCPT = std::make_unique<RCPTCommand>();
	auto DATA = std::make_unique<DATACommand>();
	auto HELP = std::make_unique<HELPCommand>();
	auto HELO = std::make_unique<HELOCommand>();
	auto RSET = std::make_unique<RSETCommand>();
	auto QUIT = std::make_unique<QUITCommand>();
	auto NOOP = std::make_unique<NOOPCommand>();
	m_commands[EHLO->GetName()] = std::move(EHLO);
	m_commands[MAIL->GetName()] = std::move(MAIL);
	m_commands[RCPT->GetName()] = std::move(RCPT);
	m_commands[DATA->GetName()] = std::move(DATA);
	m_commands[HELP->GetName()] = std::move(HELP);
	m_commands[HELO->GetName()] = std::move(HELO);
	m_commands[RSET->GetName()] = std::move(RSET);
	m_commands[QUIT->GetName()] = std::move(QUIT);
	m_commands[NOOP->GetName()] = std::move(NOOP);
}


