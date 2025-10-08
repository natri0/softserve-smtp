#include "SMTPSession.h"

ISXSMTP::SMTPSession::SMTPSession(std::unique_ptr<ITransmissionChannel> transmission_channel)
	: m_transmissionChannel(std::move(transmission_channel))
{

}

bool ISXSMTP::SMTPSession::IsFinished()
{
	if (m_context.state == ISXSMTP::SMTPStates::FINISH)
		return true;
	return false;
}
