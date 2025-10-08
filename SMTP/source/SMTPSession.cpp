#include "SMTPSession.h"

ISXSMTP::SMTPSession::SMTPSession(std::unique_ptr<ITransmissionChannel> transmission_channel)
	: m_transmissionChannel(std::move(transmission_channel))
{
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

	while (IsFinished())
	{

	}
}
