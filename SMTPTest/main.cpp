#include "SMTPSession.h"
#include "SMTPCommandParser.h"
#include "SMTPConstants.h"

#include <string>
#include <iostream>
#include <mutex>
#include <memory>

void writeToConsole(const std::string& data)
{
	for (auto i : data)
	{
		std::cout << i;
	}
}

int main(void)
{
	using namespace ISXSMTP;

	/*std::shared_ptr<MockTransmission> transmission_channel = std::make_shared<MockTransmission>();

	std::thread write_thread(input, transmission_channel);
	std::thread read_thread(console_writer, transmission_channel);*/

	SMTPSession session(nullptr);

	writeToConsole(session.OnConnect());
	writeToConsole(session.OnMessage(std::string("ehlo test")		 + SMTPConstants::CR + SMTPConstants::LF));
	writeToConsole(session.OnMessage(std::string("mail from:<test>") + SMTPConstants::CR + SMTPConstants::LF));
	writeToConsole(session.OnMessage(std::string("rcpt to:<test>")	 + SMTPConstants::CR + SMTPConstants::LF));
	writeToConsole(session.OnMessage(std::string("data")			 + SMTPConstants::CR + SMTPConstants::LF));
	writeToConsole(session.OnMessage(std::string("test mail")		 + SMTPConstants::CR + SMTPConstants::LF + '.' +SMTPConstants::CR + SMTPConstants::LF));
	writeToConsole(session.OnMessage(std::string("quit")			 + SMTPConstants::CR + SMTPConstants::LF));

	//if (session.IsFinished())
	//{
	//	// server should shutdown connection
	//}

	//SMTPString str(std::string("test"));
	//SMTPString sub_str(std::string("st"));

	//std::cout << str.FindFirstOf(sub_str);

	/*SMTPString str("HELP");
	str + ISXSMTP::SMTPConstants::CR + ISXSMTP::SMTPConstants::LF;
	auto res = ISXSMTP::CommandParser::Parse(str, session.m_commands);

	auto error_code = res.error_code.GetCode();
	std::cout << (int)error_code[0] << (int)error_code[1] << (int)error_code[2] << std::endl;
	std::cout << res.error_code.GetComment().ToString() << std::endl;

	for (auto item : res.parsed_arguments.arguments)
	{
		std::cout << "[" << item.first.ToString() << "]: " << item.second.ToString() << std::endl;
	}*/

	//write_thread.join();
	//read_thread.join();

	return 0;
}