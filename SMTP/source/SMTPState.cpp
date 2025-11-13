#include "SMTPState.h"
#include "Profiler.h"

ISXSMTP::SMTPState::SMTPState()
	: m_state(SMTPStates::INITIAL)
{
}

void ISXSMTP::SMTPState::Clear()
{
	m_state = SMTPStates::INITIAL;
}

void ISXSMTP::SMTPState::Set(SMTPStates state)
{
	m_state = state;
}

ISXSMTP::SMTPState& ISXSMTP::SMTPState::operator=(SMTPStates state)
{
	this->Set(state);
	return *this;
}

bool ISXSMTP::SMTPState::operator!=(const SMTPStates& other) const
{
	return !(*this == other);
}

bool ISXSMTP::SMTPState::operator==(const SMTPStates& other) const
{
	if (this->m_state == other)
		return true;
	return false;
}

