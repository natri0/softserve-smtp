#include "SMTPSession.h"

int main(void)
{
	using namespace ISXSMTP;

	// new connection
	SMTPSession smtp_session;
	auto result = smtp_session.ProcessClientCommand(/*socket.read()*/ std::string("NOOP") + std::string(ISXSMTP::CRLF));
	// maybe do some operations on result
	/*socket.write(result);*/


	return 0;
}