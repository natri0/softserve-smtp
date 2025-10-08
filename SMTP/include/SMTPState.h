#pragma once

namespace ISXSMTP
{

enum class SMTPStates
{
	INITIAL,
	POST_MAIL,
	POST_RCPT,
	POST_DATA,
	FINISH
};

class SMTPState
{
public:
	SMTPState();
	~SMTPState();

	void Clear();
	void Set(SMTPStates state);

public:
	bool operator==(SMTPStates other);
	bool operator!=(SMTPStates other);
	SMTPState& operator=(SMTPStates state);

private:
	SMTPStates m_state;
};

}