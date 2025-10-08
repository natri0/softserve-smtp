#include "SMTPSession.h"

ISXSMTP::SMTPSession::SMTPSession(std::unique_ptr<ITransmissionChannel> transmission_channel)
	: m_transmissionChannel(std::move(transmission_channel))
{

}

bool ISXSMTP::SMTPSession::IsFinished()
{
	// TODO
	return false;
}
