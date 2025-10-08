#pragma once

#include "ITransmissionChannel.h"
#include "SMTPContext.h"

#include <memory>
#include <unordered_map>

namespace ISXSMTP
{

class SMTPSession
{
private:
	typedef typename size_t index_t;

private:
	std::unique_ptr<ITransmissionChannel> m_transmissionChannel;
	SMTPContext m_context;
	//std::unordered_map<index_t, SMTPCommandBase> m_commands;

public:
	SMTPSession(std::unique_ptr<ITransmissionChannel> transmission_channel);

	// Returns true if SMTPSession is finished processing request
	// Returns false otherwise
	bool IsFinished();

private:
	void process();
};

}