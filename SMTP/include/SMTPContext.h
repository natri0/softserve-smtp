#pragma once

#include "SMTPBuffer.h"
#include "SMTPState.h"

namespace ISXSMTP
{

struct SMTPContext
{
	SMTPBuffer reverse_path;
	SMTPBuffer forward_path;
	SMTPBuffer mail_data;
	SMTPState state;
};

}