#pragma once

namespace ISXSMTP
{

enum class SMTPStates
{
	INITIAL,
	POST_MAIL,
	POST_RCPT,
	POST_DATA
};

class SMTPState
{
public:
	SMTPState();
	~SMTPState();

	void Clear();
	void Set(SMTPStates state);

private:
	SMTPStates m_state;
};

}