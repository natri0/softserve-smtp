#include "SMTPSession.h"
#include "SMTPCommandParser.h"
#include "SMTPConstants.h"

#include <string>
#include <iostream>
#include <mutex>
#include <memory>

void writeToConsole(std::vector<std::uint8_t> data)
{
	for (auto i : data)
	{
		std::cout << i;
	}
}

std::vector<std::uint8_t> writeToSession(const std::string& msg)
{
	std::cout << msg << std::endl;
	std::vector<std::uint8_t> data(msg.begin(), msg.end());
	data.push_back(ISXSMTP::SMTPConstants::CR);
	data.push_back(ISXSMTP::SMTPConstants::LF);
	data.push_back('.');
	data.push_back(ISXSMTP::SMTPConstants::CR);
	data.push_back(ISXSMTP::SMTPConstants::LF);
	return data;
}

int main(void)
{
	using namespace ISXSMTP;

	/*std::shared_ptr<MockTransmission> transmission_channel = std::make_shared<MockTransmission>();

	std::thread write_thread(input, transmission_channel);
	std::thread read_thread(console_writer, transmission_channel);*/

	SMTPSession session(nullptr);

	writeToConsole(session.OnConnect());
	writeToConsole(session.OnMessage(writeToSession("ehlo test")));
	writeToConsole(session.OnMessage(writeToSession("mail from:<test>")));
	writeToConsole(session.OnMessage(writeToSession("rcpt to:<test>")));
	writeToConsole(session.OnMessage(writeToSession("data")));
	writeToConsole(session.OnMessage(writeToSession("test mail")));
	writeToConsole(session.OnMessage(writeToSession("quit")));

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