#include "SMTPState.h"
#include "Profiler.h"

ISXSMTP::SMTPState::SMTPState()
	: m_state(SMTPStates::INITIAL)
{
	PROFILE_FUNC();
}

void ISXSMTP::SMTPState::Clear()
{
	PROFILE_FUNC();
	m_state = SMTPStates::INITIAL;
}

void ISXSMTP::SMTPState::Set(SMTPStates state)
{
	PROFILE_FUNC();
	m_state = state;
}

ISXSMTP::SMTPState& ISXSMTP::SMTPState::operator=(SMTPStates state)
{
	PROFILE_FUNC();
	this->Set(state);
	return *this;
}

bool ISXSMTP::SMTPState::operator!=(const SMTPStates& other) const
{
	PROFILE_FUNC();
	return !(*this == other);
}

bool ISXSMTP::SMTPState::operator==(const SMTPStates& other) const
{
	PROFILE_FUNC();
	if (this->m_state == other)
		return true;
	return false;
}

