#include "SMTPSession.h"
#include "ITransmissionChannel.h"

class MockTransmission : public ISXSMTP::ITransmissionChannel
{
	size_t Write(std::uint8_t* data, size_t count) override
	{
		return 0;
	}

	size_t Read(std::uint8_t* buffer, size_t count) override
	{
		return 0;
	}

	bool IsDataAvailable() override
	{
		return false;
	}
};

int main(void)
{
	using namespace ISXSMTP;

	SMTPSession session(std::make_unique<MockTransmission>());


	return 0;
}