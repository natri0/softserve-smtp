#pragma once

#include "SMTPBuffer.h"
#include "SMTPState.h"

namespace ISXSMTP
{

/**
 * @brief Struct that contains buffers and state on which SMTP is operating
 */
struct SMTPContext
{
	SMTPBuffer reverse_path;
	SMTPBuffer forward_path;
	SMTPBuffer mail_data;
	SMTPState state;
	bool is_authenticated = false;
};

}
