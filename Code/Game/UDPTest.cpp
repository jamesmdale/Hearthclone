#include "Game\UDPTest.hpp"
#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Core\WindowsCommon.hpp"
#include "Engine\Core\EngineCommon.hpp"
#include "Engine\Core\StringUtils.hpp"
#include "Engine\Core\DevConsole.hpp"
#include "Engine\Net\RemoteCommandService.hpp"
#include "Engine\Net\NetSession.hpp"

bool UDPTest::Start()
{
	NetAddress address;
	bool success = GetLocalIP(&address, (int)GAME_PORT);

	if (!success)
	{
		DevConsolePrintf("Address not available");
		return false;
	}

	if (!m_socket.Bind(address, 10))
	{
		DevConsolePrintf("Failed to bind.");
		return false;
	}
	else
	{
		m_socket.SetBlocking(false);
		DevConsolePrintf("Socket bound: %s", m_socket.GetAddress().ToString().c_str());
		return true;
	}

	//uint count = GetL
};

void UDPTest::Stop()
{
	m_socket.Close();
};

void UDPTest::SendTo(const NetAddress& addr, const void* buffer, uint byteCount)
{
	m_socket.SendTo(addr, buffer, byteCount);
};

void UDPTest::Update()
{
	byte_t buffer[PACKET_MTU];
	NetAddress fromAddress;

	size_t read = m_socket.Receive(&fromAddress, buffer, PACKET_MTU);

	if (read > 0U) 
	{
		unsigned int max_bytes = GetMinInt(read, (size_t) 128);
		unsigned int string_size = max_bytes * 2U + 3U;
		char* buffer = new char[string_size];
		sprintf_s (buffer, 3U, "0x");
		byte_t *iter = (byte_t*) buffer;
		iter += 2U;

		for (unsigned int i = 0; i <read; ++i)
		{
			sprintf_s((char*) iter, 3U, "%02X", buffer[i]);
			iter+=2U;
		}
		*iter = NULL;

		DevConsolePrintf( "Received from %s;\n%s", fromAddress.ToString().c_str(), buffer );
	}
};