#include "SMTPState.h"

ISXSMTP::SMTPState::SMTPState()
	: m_state(SMTPStates::INITIAL)
{

}

ISXSMTP::SMTPState::~SMTPState()
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

