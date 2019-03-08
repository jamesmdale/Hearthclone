#pragma once
#include "Engine\Time\Clock.hpp"
#include "Engine\Renderer\ForwardRenderingPath2D.hpp"
#include "Engine\Net\NetSession.hpp"
#include "Engine\Camera\Camera.hpp"
#include "Game\Definitions\CardDefinition.hpp"
#include "Game\Definitions\DeckDefinition.hpp"
#include "Game\GameCommon.hpp"
#include <vector>

class NetMessage;
class NetConnection;
class RNG;
enum eNetMessageFlag;

enum eGameNetMessageType
{
	//start at end of other message type list
	TEST_GAME_NET_MESSAGE_TYPE = NUM_CORE_NET_MESSAGE_TYPES,

	//... more types
	UNRELAIBLE_TEST_GAME_NET_MESSAGE_TYPE = 128,
	RELIABLE_TEST_GAME_NET_MESSAGE_TYPE = 129,
	SEQUENCE_TEST_GAME_NET_MESSAGE_TYPE = 130
};

class Game
{
public:
	Game();
	~Game();
	static Game* GetInstance();
	static Game* CreateInstance();
	static RNG* GetGlobalRNG();
	static void ResetGlobalRNG();

	void Initialize();

	void Update(); //use internal clock for delta seconds;

	void PreRender();
	void Render();
	void PostRender();

	float UpdateInput(float deltaSeconds);

	//network
	void RegisterGameNetMessages();	

	void TestReliableSend();

	//game functions
	void LoadDefaultDeck();

public:
	//camera members
	Camera* m_gameCamera = nullptr;

	//rendering members
	ForwardRenderingPath2D* m_forwardRenderingPath2D = nullptr;

	//clock
	Clock* m_gameClock = nullptr;

	//game members
	DeckDefinition* m_playerLoadedDeckDefinition = nullptr;
	DeckDefinition* m_enemyLoadedDeckDefinition = nullptr;
	bool m_isHosting = false;

	NetConnection* m_myConnection = nullptr;
	NetConnection* m_enemyConnection = nullptr;

	std::string m_hostAddress = "";
	Stopwatch* m_reliableSendTimer = nullptr;
	Stopwatch* m_netResendTimer = nullptr;
};

//game commands
void LoadDeck(Command& cmd);
void ChangeHost(Command& cmd);

//net commands
void UnreliableTest(Command& cmd);
void ReliableTest(Command& cmd);
void OutOfOrderTest(Command& cmd);
void ReliableSequenceTest(Command& cmd);

//Net message definition callbacks
bool OnUnreliableTest(NetMessage& message, NetConnection* fromConnection);
bool OnReliableTest(NetMessage& message, NetConnection* fromConnection);
bool OnSequenceTest(NetMessage& message, NetConnection* fromConnection);
bool OnTest(NetMessage& message, NetConnection* fromConnection);







