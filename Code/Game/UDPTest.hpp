#include "Engine\Net\UDPSocket.hpp"
#include "Engine\Net\NetAddress.hpp"
#include "Engine\Core\EngineCommon.hpp"


//// class test
//#define GAME_PORT 10084
//#define ETHERNET_MTU 1500  // maximum transmission unit - determined by hardware part of OSI model.
//// 1500 is the MTU of EthernetV2, and is the minimum one - so we use it; 
//#define PACKET_MTU (ETHERNET_MTU - 40 - 8) 

// IPv4 Header Size: 20B
// IPv6 Header Size: 40B
// TCP Headre Size: 20B-60B
// UDP Header Size: 8B 
// Ethernet: 28B, but MTU is already adjusted for it
// so packet size is 1500 - 40 - 8 => 1452B (why?)

class UDPTest
{
public:
	UDPSocket m_socket;

public:
	bool Start();
	void Stop();
	void SendTo(const NetAddress& addr, const void* buffer, uint byteCount);
	void Update();
};