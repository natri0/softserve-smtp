#pragma once

namespace ISXSMTP
{

/**
 * @brief Enum with all possible states that SMTPSession could be in
 */
enum class SMTPStates
{
	INITIAL,
	POST_EHLO,
	POST_MAIL,
	POST_RCPT,
	POST_DATA,
	END_DATA,
	FINISH
};

/**
 * @brief Class wrapper for the SMTPStates
 *	This class may be removed in the future and plain enum will be used instead
 */
class SMTPState
{
private:
	SMTPStates m_state;

public:
	SMTPState();

	void Clear();
	void Set(SMTPStates state);

public:
	bool operator==(const SMTPStates& other) const;
	bool operator!=(const SMTPStates& other) const;
	SMTPState& operator=(SMTPStates state);
};

}
