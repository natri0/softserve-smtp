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
#include "CommandParser.h"

#include <iostream>

ISXSMTP::SMTPSession::SMTPSession(std::shared_ptr<ITransmissionChannel> transmission_channel)
	: m_transmissionChannel(transmission_channel)
{
	fillCommandMap();
	m_context = std::make_shared<SMTPContext>();
	process();
}

bool ISXSMTP::SMTPSession::IsFinished()
{
	if (m_context->state == ISXSMTP::SMTPStates::FINISH)
		return true;
	return false;
}

void ISXSMTP::SMTPSession::process()
{
	/*
	* 1. Read data from transmission channel
	* 2. When <CLRF> discovered end reading and pass line to Parser
	* 3. Call corresponding command with parsed arguments
	* 4. Write SMTPReply code to user
	* 5. Repeat until QUIT occurs
	*/

	while (!IsFinished())
	{
		if (m_transmissionChannel->IsDataAvailable())
		{
			SMTPString command;
			do 
			{
				m_transmissionChannel->Read(command.GetData());
			} while (command.Count() != 0 && command.Get(command.Count() - 1) != SMTPConstants::LF);

			if (!command.IsEmpty())
			{
				SMTPString output;
				auto command_parser_result = ISXSMTP::CommandParser::Parse(command, m_commands);

				if (command_parser_result.error_code != SMTPReply::OK())
				{
					for (auto i : command_parser_result.error_code.GetCode())
					{
						char ch;
						itoa(i, &ch, 10);
						output.Append(ch);
					}

					output.Append(" ");
					output.Append(command_parser_result.error_code.GetComment());
					output.Append('\n');

					m_transmissionChannel->Write(output.GetData());
				}
				else
				{
					auto command_result = m_commands[command_parser_result.command_verb]->Invoke(
							command_parser_result.parsed_arguments,
							m_context);

					for (auto i : command_result.GetCode())
					{
						char ch[2];
						itoa(i, ch, 10);
						output.Append(ch[0]);
					}

					output.Append(" ");
					output.Append(command_result.GetComment());
					output.Append('\n');

					m_transmissionChannel->Write(output.GetData());
				}	
			}
		}
	}
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
