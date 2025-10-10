#include "SMTPSession.h"
#include "ITransmissionChannel.h"
#include "CommandParser.h"
#include "SMTPConstants.h"

#include <string>
#include <iostream>
#include <mutex>
#include <memory>

class MockTransmission : public ISXSMTP::ITransmissionChannel
{
private:
	std::mutex m_mutex;
	std::vector<std::uint8_t> m_buffer;
	std::vector<std::uint8_t> m_outsideBuffer;

public:
	size_t Write(std::vector<uint8_t>& buffer) override
	{
		std::scoped_lock(m_mutex);
		m_outsideBuffer.clear();
		m_outsideBuffer.append_range(buffer);
		return m_outsideBuffer.size();
	}

	size_t WriteOutside(std::vector<uint8_t>& buffer) 
	{
		std::scoped_lock(m_mutex);
		m_buffer.clear();
		m_buffer.append_range(buffer);
		return m_buffer.size();
	}


	size_t Read(std::vector<uint8_t>& buffer) override
	{
		std::scoped_lock(m_mutex);
		buffer.clear();
		buffer.append_range(m_buffer);
		m_buffer.clear();
		return buffer.size();
	}

	size_t ReadOutside(std::vector<uint8_t>& buffer) 
	{
		std::scoped_lock(m_mutex);
		buffer.clear();
		buffer.append_range(m_outsideBuffer);
		m_outsideBuffer.clear();
		return buffer.size();
	}

	bool IsDataAvailable() override
	{
		std::scoped_lock lock(m_mutex);
		return !m_buffer.empty();
	}

	bool IsDataAvailableOutside() 
	{
		std::scoped_lock lock(m_mutex);
		return !m_outsideBuffer.empty();
	}

};

void input(std::shared_ptr<MockTransmission> transmission_channel)
{
	while (true)
	{
		std::string input;
		std::cin >> input;
		std::vector<std::uint8_t> buffer(input.begin(), input.end());
		buffer.push_back(ISXSMTP::SMTPConstants::CR);
		buffer.push_back(ISXSMTP::SMTPConstants::LF);
		transmission_channel->WriteOutside(buffer);
	}
}

void console_writer(std::shared_ptr<MockTransmission> transmission_channel)
{
	std::vector<std::uint8_t> buffer;
	while (true)
	{
		if (transmission_channel->IsDataAvailableOutside())
		{
			size_t bytes_read = transmission_channel->ReadOutside(buffer);
			std::cout << "Bytes read: " << bytes_read << std::endl;
			for (size_t i = 0; i < bytes_read; i++)
			{
				std::cout << buffer[i];
			}
			std::cout << std::endl;
		}
	}
}

int main(void)
{
	using namespace ISXSMTP;

	std::shared_ptr<MockTransmission> transmission_channel = std::make_shared<MockTransmission>();

	std::thread write_thread(input, transmission_channel);
	std::thread read_thread(console_writer, transmission_channel);

	SMTPSession session(transmission_channel);

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

	return 0;
}