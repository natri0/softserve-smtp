#pragma once

#include "ITransmissionChannel.h"
#include "SMTPContext.h"
#include "Commands/SMTPCommandBase.h"

#include <memory>
#include <unordered_map>


namespace ISXSMTP
{

class SMTPSession
{
private:
	std::shared_ptr<ITransmissionChannel> m_transmissionChannel;
	std::shared_ptr<SMTPContext> m_context;
public: // temporary for testing
	std::unordered_map<SMTPString, std::unique_ptr<SMTPCommandBase>> m_commands;

public:
	SMTPSession(std::shared_ptr<ITransmissionChannel> transmission_channel);

	// Returns true if SMTPSession is finished processing request
	// Returns false otherwise
	bool IsFinished();

private:
	void process();
	void fillCommandMap();
};

}